#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <glm/glm.hpp>
#include <algorithm> // std::clamp
#include <limits>    // float minimum
#include <stack>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();

constexpr size_t TRACE_LIMIT = 64;
constexpr float ROULETTE_HIT_RATE = 0.25f;

bool IsEmitting(const unsigned int geomID) {
  
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
  RussianRoulette roulette(TRACE_LIMIT, ROULETTE_HIT_RATE);
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGLÇÃï`âÊÇÕleft-bottom-upÇ»ÇÃÇ≈ãtÇ…Ç∑ÇÈ
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = pixelPos - cameraPos;
      RTCRayHit rayhit;
      InitRayHit(rayhit, cameraPos, rayDir);

      //scene.GetGeomColor(rayhit.hit.geomID, 0.0f, 0.0f)

      // ÉåÉìÉ_ÉäÉìÉOï˚íˆéÆÇÃç∂ï”
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f };

      while (!roulette.Spin() && WasIntersected(rayhit.hit.geomID)) {
        glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
        
      }
      cumulativeRadiance.r = std::clamp(cumulativeRadiance.r, 0.0f, 1.0f);
      cumulativeRadiance.g = std::clamp(cumulativeRadiance.g, 0.0f, 1.0f);
      cumulativeRadiance.b = std::clamp(cumulativeRadiance.b, 0.0f, 1.0f);
      renderTarget[yIdx][xIdx].r = static_cast<uint8_t>((cumulativeRadiance.r * 255.0f));
      renderTarget[yIdx][xIdx].g = static_cast<uint8_t>((cumulativeRadiance.g * 255.0f));
      renderTarget[yIdx][xIdx].b = static_cast<uint8_t>((cumulativeRadiance.b * 255.0f));
    }
  }
}

} // namespace cges::renderer