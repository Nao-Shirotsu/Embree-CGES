#include "renderer_phongshader.hpp"
#include "util_renderer.hpp"
#include "util_general.hpp"
#include "color.hpp"

#include <glm/glm.hpp>
#include <algorithm> // for std::clamp

namespace {

constexpr cges::ColorRGBA EMIT_COLOR = { 100, 100, 100, 255 };
constexpr cges::ColorRGBA BG_COLOR = { 64, 128, 128, 255 };
constexpr cges::ColorRGBf BG_COLOR_F = { 0.25f, 0.5f, 0.5f };

constexpr float SPECULAR_BASE_INTENSITY = 1.0f / 4.0f + 1.0f / 8.0f;

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
      const float xRate = x / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = glm::normalize(pixelPos - cameraPos);
      renderTarget[yIdx][x] = ToColorUInt(ComputeLightTransport(scene, pixelPos, rayDir));
    }
  }
}
ColorRGBf PhongShader::ComputeLightTransport(const Scene& scene, const glm::vec3& org, const glm::vec3& dir) const {
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  RTCRayHit rayhit;
  InitRayHit(rayhit, org, dir);

  rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

  if (!WasIntersected(rayhit.hit.geomID)) {
    return BG_COLOR_F;
  }

  const auto& gameObjRef = scene.GetGeomRef(rayhit.hit.geomID);

  // shading
  glm::vec3 faceNormal = { 0, 0, 0 };
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

  const glm::vec3 reflectedDir = dir - 2.0f * glm::dot(dir, faceNormal) * faceNormal;
  const float specularFactor = std::clamp(std::fabsf(glm::dot(glm::normalize(reflectedDir), glm::normalize(org))), 0.0f, 1.0f);
  const float diffuseFactor = std::clamp(std::fabsf(glm::dot(-dir, faceNormal)), 0.0f, 1.0f);
  const ColorRGBA objAlbedo = gameObjRef.GetColor(rayhit.hit.u, rayhit.hit.v);

  return {
    Saturate(ToFloat(objAlbedo.r) * diffuseFactor + SPECULAR_BASE_INTENSITY * specularFactor),
    Saturate(ToFloat(objAlbedo.g) * diffuseFactor + SPECULAR_BASE_INTENSITY * specularFactor),
    Saturate(ToFloat(objAlbedo.b) * diffuseFactor + SPECULAR_BASE_INTENSITY * specularFactor),
  };
}
} // namespace cges::renderer
