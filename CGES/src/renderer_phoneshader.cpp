#include "renderer_phoneshader.hpp"
#include "util_renderer.hpp"
#include "color.hpp"
#include "util_renderer.hpp"

#include <glm/glm.hpp>
#include <algorithm> // for std::clamp

namespace {

constexpr cges::ColorRGBA EMIT_COLOR = { 100, 100, 100, 255 };

}

namespace cges::renderer {
PhongShader::PhongShader() : Base(Method::PhongShading){
}
void PhongShader::Update(const Camera& camera) {}

void PhongShader::ParallelDraw(const Camera& camera,
                               RenderBuffer& renderTarget,
                               const Scene& scene,
                               const size_t loopMin,
                               const size_t loopMax,
                               const glm::vec3& initialPos,
                               const glm::vec3& screenVerticalVec,
                               const glm::vec3& screenHorizontalVec) const {
  const auto cameraPos = camera.posWorld + camera.GetPosLocal();
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1;
    for (size_t x = 0; x < renderTarget.GetWidth(); ++x) {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      const float xRate = x / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = glm::normalize(pixelPos - cameraPos);
      RTCRayHit rayhit;
      InitRayHit(rayhit, cameraPos, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][x].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][x].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][x].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }
      const auto& gameObjRef = scene.GetGeomRef(rayhit.hit.geomID);
      renderTarget[yIdx][x] = gameObjRef.GetColor(0.0f, 0.0f);

      // shading
      glm::vec3 faceNormal = {0, 0, 0};
      if (IsInterpolatable(gameObjRef.GetGeomType())) {
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

      const glm::vec3 reflectedDir = rayDir - 2.0f * glm::dot(rayDir, faceNormal) * faceNormal;
      const float specularFactor = std::clamp(std::fabsf(glm::dot(glm::normalize(reflectedDir), glm::normalize(cameraPos))), 0.0f, 1.0f);
      const float diffuseFactor = std::clamp(std::fabsf(glm::dot(-rayDir, faceNormal)), 0.0f, 1.0f);
      const ColorRGBA objColor = gameObjRef.GetColor(rayhit.hit.u, rayhit.hit.v);
      renderTarget[yIdx][x].r = std::clamp(static_cast<int>(objColor.r * diffuseFactor + 96 * specularFactor) + 32, 0, 255);
      renderTarget[yIdx][x].g = std::clamp(static_cast<int>(objColor.g * diffuseFactor + 96 * specularFactor) + 16, 0, 255);
      renderTarget[yIdx][x].b = std::clamp(static_cast<int>(objColor.b * diffuseFactor + 96 * specularFactor) + 16, 0, 255);

      // shadowing
      const glm::vec3 hitPos = cameraPos + rayhit.ray.tfar * glm::vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
      for (const auto& idx : scene.GetLightIndices()) {
        auto shadowRayDir = glm::normalize(scene.GetGeomRef(idx).SampleSurfacePoint() - hitPos);
        RTCRayHit shadowRayhit;
        InitRayHit(shadowRayhit, cameraPos + (0.0001f * shadowRayDir), shadowRayDir);
        rtcIntersect1(scene.GetRTCScene(), &context, &shadowRayhit);
        if (shadowRayhit.hit.geomID == idx) {
          continue;
        }
        renderTarget[yIdx][x].r *= 0.75f;
        renderTarget[yIdx][x].g *= 0.75f;
        renderTarget[yIdx][x].b *= 0.75f;
      }
      
    }
  }
}
} // namespace cges::renderer
