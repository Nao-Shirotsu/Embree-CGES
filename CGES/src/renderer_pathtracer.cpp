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
constexpr size_t TRACE_LOWER_LIMIT = 5;
constexpr size_t TRACE_UPPER_LIMIT = 64;
constexpr size_t NUM_SAMPLING = 32;

uint8_t ToByte(const float intensity) {
  return static_cast<uint8_t>(std::powf(std::clamp(intensity, 0.0f, 1.0f), 1.0f / 2.2f) * 255.0f + 0.5f);
}

// 色の強さは[0.0, 1.0]で返す
glm::vec3 ComputeRadiance(RTCIntersectContext& context,
                          const glm::vec3& rayOrg,
                          const glm::vec3& incomingDir,
                          cges::RussianRoulette& roulette,
                          const cges::Scene& scene) {
  using namespace cges;

  WrappedRayHit rayhit(rayOrg, incomingDir);
  if (!rayhit.Intersect(scene, context)) {
    return { 0.0f, 0.0f, 0.0f };
  }

  const auto& intersectedObj = scene.GetGeomRef(rayhit.HitGeomId());
  const glm::vec3 surfaceEmission = {
    static_cast<float>(intersectedObj.GetEmission().r) / 255.0f,
    static_cast<float>(intersectedObj.GetEmission().g) / 255.0f,
    static_cast<float>(intersectedObj.GetEmission().b) / 255.0f
  };

  const float texU = rayhit.HitU();
  const float texV = rayhit.HitV();
  const glm::vec3 surfaceAlbedo = {
    static_cast<float>(intersectedObj.GetColor(texU, texV).r) / 255.0f,
    static_cast<float>(intersectedObj.GetColor(texU, texV).g) / 255.0f,
    static_cast<float>(intersectedObj.GetColor(texU, texV).b) / 255.0f
  };

  {
    const auto continueRate = std::max(std::max(surfaceAlbedo.r, surfaceAlbedo.g), surfaceAlbedo.b);
    roulette.SetContinueRate(continueRate);
  }

  if (roulette.Spin()) {
    return surfaceEmission;
  }

  const glm::vec3 faceNormal = glm::normalize(rayhit.HitNormal());
  const glm::vec3 surfacePos = rayOrg + rayhit.RayTfar() * incomingDir;
  const glm::vec3 outgoingDir = intersectedObj.ComputeReflectedDir(faceNormal, incomingDir);
  const glm::vec3 radianceWeight = intersectedObj.IntegrandFactor(surfacePos, outgoingDir, incomingDir, faceNormal, surfaceAlbedo) / roulette.ContinueRate();
  //const glm::vec3 brdfValue = intersectedObj.BRDF(surfacePos, outgoingDir, incomingDir, faceNormal, surfaceAlbedo);

  return surfaceEmission + radianceWeight * ComputeRadiance(context, surfacePos, outgoingDir, roulette, scene);
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
      WrappedRayHit rayhit(rayOrg, rayDir);
      
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      // primal rayを飛ばして何にも衝突しなかったら背景色を描画して次のピクセルへ
      if (!rayhit.Intersect(scene, context)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // primal rayが何かに衝突した時、レンダリング方程式の右辺を再帰的に計算する
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f }; // MC積分の総和項(あとでサンプリング回数で割る)
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        RussianRoulette roulette(randomGen, TRACE_LOWER_LIMIT, TRACE_UPPER_LIMIT, INITIAL_ROULETTE_HIT_RATE);
        const auto radi = ComputeRadiance(context, rayOrg, rayDir, roulette, scene);
        cumulativeRadiance += radi;
      }

      cumulativeRadiance = cumulativeRadiance / static_cast<float>(NUM_SAMPLING);

      renderTarget[yIdx][xIdx].r = ToByte(cumulativeRadiance.r);
      renderTarget[yIdx][xIdx].g = ToByte(cumulativeRadiance.g);
      renderTarget[yIdx][xIdx].b = ToByte(cumulativeRadiance.b);

      if (xIdx > 1 && renderTarget [yIdx][xIdx].r == 0 && renderTarget[yIdx][xIdx].g == 0 && renderTarget[yIdx][xIdx].b == 0 && renderTarget[yIdx][xIdx - 1].r == 0 && renderTarget[yIdx][xIdx - 1].g == 0 && renderTarget[yIdx][xIdx - 1].b == 0) {
        int a = 5;
      }
      
    }
  }
}

} // namespace cges::renderer