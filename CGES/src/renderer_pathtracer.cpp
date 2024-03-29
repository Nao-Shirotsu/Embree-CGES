#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"
#include "stopwatch.hpp"

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
  const glm::vec3 surfacePos = rayOrg + rayhit.RayTfar() * incomingDir + faceNormal * (EPSILON * 100); // o+td+始点オフセット
  const glm::vec3 outgoingDir = intersectedObj.ComputeReflectedDir(faceNormal, incomingDir);
  const glm::vec3 radianceWeight = intersectedObj.IntegrandFactor(surfacePos, outgoingDir, incomingDir, faceNormal, surfaceAlbedo) / roulette.ContinueRate();

  return surfaceEmission + radianceWeight * ComputeRadiance(context, surfacePos, outgoingDir, roulette, scene);
}

}

namespace cges::renderer {

PathTracer::PathTracer(const size_t traceLowerLimit, const size_t traceUpperLimit, const size_t samplingLimit)
    : Base(traceLowerLimit, traceUpperLimit, samplingLimit, Method::Pathtracing) {
}

void PathTracer::Update(const Camera& camera) {
  if (camera.JustMoved()) {
    m_numSampling = 1;
  }
  if (m_numSampling > m_samplingLimit) {
    return;
  }
  std::cout << "Pathtracing (" << m_numSampling << " samples)" << std::endl;
  m_numSampling *= 2;
}

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

  const float onePixelSizeRateX = 1.0f / static_cast<float>(renderTarget.GetWidth());
  const float onePixelSizeRateY = 1.0f / static_cast<float>(renderTarget.GetHeight());

  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGLの描画はleft-bottom-upなので逆にする
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);

      const glm::vec3 rayDir = pixelPos - cameraPos;
      const glm::vec3 rayOrg = cameraPos;
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
      for (auto sampling = 0u; sampling < m_numSampling; ++sampling) {
        RussianRoulette roulette(randomGen, m_traceLowerLimit, m_traceUpperLimit, INITIAL_ROULETTE_HIT_RATE);

        // スーパーサンプリング
        const float ssYRate = yRate + randomGen() * onePixelSizeRateY;
        const float ssXRate = xRate + randomGen() * onePixelSizeRateX;
        const auto ssPixelPos = initialPos + (ssYRate * screenVerticalVec) + (ssXRate * screenHorizontalVec);
        const auto ssRayDir = ssPixelPos - cameraPos;

        // MC積分のサンプリング1回分
        cumulativeRadiance += ComputeRadiance(context, rayOrg, ssRayDir, roulette, scene);
      }

      cumulativeRadiance = cumulativeRadiance / static_cast<float>(m_numSampling);

      renderTarget[yIdx][xIdx].r = ToByte(cumulativeRadiance.r);
      renderTarget[yIdx][xIdx].g = ToByte(cumulativeRadiance.g);
      renderTarget[yIdx][xIdx].b = ToByte(cumulativeRadiance.b);
    }
  }
}

} // namespace cges::renderer