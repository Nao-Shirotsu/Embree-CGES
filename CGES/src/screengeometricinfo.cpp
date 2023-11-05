#include "screengeometricinfo.hpp"

#include "util_general.hpp"

#include <glm/glm.hpp>

namespace {

constexpr float SCREEN_GEOMETRIC_WIDTH = 0.25f;

float DegToRad(const float degree) {
  return degree * cges::constants::PI / 180.0f;
}

}// anon namespace

namespace cges {

ScreenGeometricInfo::ScreenGeometricInfo(const Camera& camera, const RenderBuffer& renderTarget)
  : ref_camera(camera)
  , ref_renderTarget(renderTarget)
  , m_aspectRatio(renderTarget.GetHeight() / static_cast<float>(renderTarget.GetWidth()))
  , m_horizontVec(glm::normalize(glm::cross(camera.upwardWorld, camera.GetFrontDir())) * SCREEN_GEOMETRIC_WIDTH)
  , m_verticalVec(glm::normalize(glm::cross(m_horizontVec, camera.GetFrontDir())) * SCREEN_GEOMETRIC_WIDTH * m_aspectRatio)
  , m_distanceCamera2Screen(std::tanf(DegToRad((180.0f - camera.fov) / 2.0f)) * SCREEN_GEOMETRIC_WIDTH / 2.0f)
  , m_centerPos(camera.posWorld + camera.GetPosLocal() + ref_camera.GetFrontDir() * m_distanceCamera2Screen)
  , m_upleftPos(m_centerPos - (glm::vec3{ 0.5, 0.5, 0.5 } * m_verticalVec) - (glm::vec3{ 0.5, 0.5, 0.5 } * m_horizontVec)) 
  {}

glm::vec3 ScreenGeometricInfo::ComputeDirection(const unsigned int y, const unsigned int x) const noexcept {
  glm::vec3 cameraPos = ref_camera.posWorld + ref_camera.GetPosLocal();
  return glm::normalize(ComputePixelPos(y, x) - cameraPos);
}

glm::vec3 ScreenGeometricInfo::ComputePixelPos(const unsigned int y, const unsigned int x) const noexcept {
  const float yRate = y / static_cast<float>(ref_renderTarget.GetHeight());
  const float xRate = x / static_cast<float>(ref_renderTarget.GetWidth());
  return m_upleftPos + (yRate * m_verticalVec) + (xRate * m_horizontVec);
}

} // namespace cges