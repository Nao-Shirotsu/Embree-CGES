#include "renderer_photonmapper.hpp"

#include "util_renderer.hpp"

namespace cges::renderer {

PhotonMapper::PhotonMapper()
    : Base(Method::PhotonMapping) {}

void PhotonMapper::Update(const Camera& camera) {
}

void PhotonMapper::ParallelDraw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene, const size_t loopMin, const size_t loopMax, const glm::vec3& initialPos, const glm::vec3& screenVerticalVec, const glm::vec3& screenHorizontalVec) const {
  const auto cameraPos = camera.posWorld + camera.GetPosLocal();
  RandomGenerator randomGen;

  const float onePixelSizeRateX = 1.0f / static_cast<float>(renderTarget.GetWidth());
  const float onePixelSizeRateY = 1.0f / static_cast<float>(renderTarget.GetHeight());

  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGL�̕`���left-bottom-up�Ȃ̂ŋt�ɂ���
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);

      const glm::vec3 rayDir = pixelPos - cameraPos;
      const glm::vec3 rayOrg = cameraPos;
      WrappedRayHit rayhit(rayOrg, rayDir);

      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      // primal ray���΂��ĉ��ɂ��Փ˂��Ȃ�������w�i�F��`�悵�Ď��̃s�N�Z����
      if (!rayhit.Intersect(scene, context)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // primal ray�������ɏՓ˂������A�����_�����O�������̉E�ӂ��ċA�I�Ɍv�Z����
      
    }
  }
}

} // namespace cges::renderer