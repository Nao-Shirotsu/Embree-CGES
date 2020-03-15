#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <glm/glm.hpp>
#include <algorithm> // std::clamp
#include <limits>    // float minimum

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();

constexpr size_t TRACE_LIMIT = 64;
constexpr float ROULETTE_HIT_RATE = 0.25f;

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
  RussianRoulette roulette(TRACE_LIMIT, ROULETTE_HIT_RATE);
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGL‚Ì•`‰æ‚Íleft-bottom-up‚È‚Ì‚Å‹t‚É‚·‚é
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = pixelPos - cameraPos;
      RTCRayHit rayhit;
      InitRayHit(rayhit, cameraPos, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }
      renderTarget[yIdx][xIdx] = scene.GetGeomColor(rayhit.hit.geomID, 0.0f, 0.0f);

      // shading
      glm::vec3 faceNormal = { 0, 0, 0 };
      if (IsInterpolatable(scene.GetGeomType(rayhit.hit.geomID))) {
        rtcInterpolate0(rtcGetGeometry(scene.GetRTCScene(), rayhit.hit.geomID),
                        rayhit.hit.primID,
                        rayhit.hit.u,
                        rayhit.hit.v,
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                        0,
                        reinterpret_cast<float*>(&faceNormal),
                        3);
      }
      else {
        faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
      }

      // ŽÀ‘•‚·‚é
      glm::vec3 cumulativeRadiance = {0.0f, 0.0f, 0.0f};
      while (!roulette.Spin()) {
        
      }
    }
  }
}

} // namespace cges::renderer