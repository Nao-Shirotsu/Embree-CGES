#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <algorithm> // std::clamp
#include <glm/glm.hpp>
#include <limits> // float minimum
#include <stack>

#include <iostream>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();
constexpr float INITIAL_ROULETTE_HIT_RATE = 0.25f;
constexpr float PI = 3.14159265358979323846f;
constexpr unsigned int TRACE_INITIAL_ID = 1;
constexpr size_t TRACE_LIMIT = 64;
constexpr size_t NUM_SAMPLING = 32;

// 色の強さは[0.0, 1.0]で返す
glm::vec3 ComputeRadiance(RTCIntersectContext& context,
                          const glm::vec3& rayOrg,
                          const glm::vec3& incomingDir,
                          cges::RussianRoulette& roulette,
                          const cges::Scene& scene) {
  using namespace cges;

  RTCRayHit rayhit;
  InitRayHit(rayhit, rayOrg, incomingDir);
  rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

  if (!WasIntersected(rayhit.hit.geomID)) {
    return { 0.0f, 0.0f, 0.0f };
  }

  const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);
  const glm::vec3 surfaceEmission = {
    static_cast<float>(intersectedObj.GetEmission().r) / 255.0f,
    static_cast<float>(intersectedObj.GetEmission().g) / 255.0f,
    static_cast<float>(intersectedObj.GetEmission().b) / 255.0f
  };

  const glm::vec3 pointColor = {
    static_cast<float>(intersectedObj.GetColor(rayhit.hit.u, rayhit.hit.v).r) / 255.0f,
    static_cast<float>(intersectedObj.GetColor(rayhit.hit.u, rayhit.hit.v).g) / 255.0f,
    static_cast<float>(intersectedObj.GetColor(rayhit.hit.u, rayhit.hit.v).b) / 255.0f
  };

  const auto continueRate = std::max(std::max(pointColor.r, pointColor.g), pointColor.b);
  roulette.SetContinueRate(continueRate);

  if (roulette.Spin()) {
    return surfaceEmission;
  }

  const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
  const glm::vec3 surfacePos = rayOrg + rayhit.ray.tfar * incomingDir;
  const glm::vec3 outgoingDir = intersectedObj.ComputeReflectedDir(faceNormal, incomingDir);
  const glm::vec3 incomingRadiance = ComputeRadiance(context, surfacePos, outgoingDir, roulette, scene);
  //const glm::vec3 brdfValue = intersectedObj.BRDF(surfacePos, outgoingDir, incomingDir, faceNormal, pointColor);

  return surfaceEmission + incomingRadiance * pointColor / continueRate; // diffuseのみ
}

}

namespace cges::renderer {

void PathTracer::ParallelDraw(const Camera& camera,
                              RenderBuffer& renderTarget,
                              const Scene& scene,
                              const size_t loopMin,
                              const size_t loopMax,
                              const glm::vec3& initialPos,
                              const glm::vec3& screenVerticalVec,
                              const glm::vec3& screenHorizontalVec) const {
  const auto cameraPos = camera.posWorld + camera.GetPosLocal();
  RandomGenerator randomGen;
  for (size_t y = loopMin; y < loopMax; ++y) {
    std::cout << "column " << y << std::endl;
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGLの描画はleft-bottom-upなので逆にする
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      glm::vec3 rayDir = pixelPos - cameraPos;
      glm::vec3 rayOrg = cameraPos;
      RTCRayHit rayhit;
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      // primal ray を飛ばす
      InitRayHit(rayhit, rayOrg, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      // primal rayが何にも衝突しなかったら背景色を描画して次のピクセルへ
      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // primal rayが何かに衝突した時、レンダリング方程式の右辺を再帰的に計算する
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f }; // MC積分の総和項(あとでサンプリング回数で割る)
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        RussianRoulette roulette(randomGen, TRACE_LIMIT, INITIAL_ROULETTE_HIT_RATE);
        const auto radi = ComputeRadiance(context, rayOrg, rayDir, roulette, scene);
        cumulativeRadiance += radi;
      }

      cumulativeRadiance = cumulativeRadiance / static_cast<float>(NUM_SAMPLING);

      // オーバーフロー対策で一応clampする
      /*cumulativeRadiance.r = std::clamp(cumulativeRadiance.r, 0.0f, 255.0f);
      cumulativeRadiance.g = std::clamp(cumulativeRadiance.g, 0.0f, 255.0f);
      cumulativeRadiance.b = std::clamp(cumulativeRadiance.b, 0.0f, 255.0f);*/
      renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(cumulativeRadiance.r * 255.0f);
      renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(cumulativeRadiance.g * 255.0f);
      renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(cumulativeRadiance.b * 255.0f);
    }
  }
}

} // namespace cges::renderer