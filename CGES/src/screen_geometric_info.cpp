#include "screen_geometric_info.hpp"

#include "util_general.hpp"

#include <glm/glm.hpp>

namespace {
// 3������ԏ�̃X�N���[���I�u�W�F�N�g�̉���
constexpr float SCREEN_WIDTH = 0.25f;
}

cges::ScreenGeometricInfo::ScreenGeometricInfo(const Camera& camera, const float aspectRatio) noexcept 
{
  const glm::vec3 cameraFront = glm::normalize(glm::vec3{ 0, 0, 0 } - camera.GetPosLocal());
  m_horizontVec = glm::normalize(glm::cross(camera.upwardWorld, cameraFront)) * SCREEN_WIDTH;
  m_verticalVec = glm::normalize(glm::cross(m_horizontVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
  const auto distanceToScreen = std::tanf(DegToRad((180.0f - camera.fov) / 2.0f)) * SCREEN_WIDTH / 2.0f;
  m_centerPos = camera.posWorld + camera.GetPosLocal() + cameraFront * distanceToScreen;

  // �X�N���[������(pixel[0][0])�̃��[���h���W
  m_upleftPos = m_centerPos - (glm::vec3{ 0.5, 0.5, 0.5 } * m_verticalVec) - (glm::vec3{ 0.5, 0.5, 0.5 } * m_horizontVec);
}

const glm::vec3& cges::ScreenGeometricInfo::GetCenterPos() const noexcept {
  return m_centerPos;
}

const glm::vec3& cges::ScreenGeometricInfo::GetUpleftPos() const noexcept {
  return m_upleftPos;
}

const glm::vec3& cges::ScreenGeometricInfo::GetHorizontVec() const noexcept {
  return m_horizontVec;
}

const glm::vec3& cges::ScreenGeometricInfo::GetVerticalVec() const noexcept {
  return m_verticalVec;
}
