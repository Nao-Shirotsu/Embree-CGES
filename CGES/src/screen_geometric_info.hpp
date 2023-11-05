#pragma once

#include "Camera.hpp"

#include <glm/vec3.hpp>

namespace cges {

// RenderBuffer�̊􉽊w�I�p�����[�^�\����
// ���[���h���W�ɕ����Ă���X�N���[���o�b�t�@�̈ʒu�Ƃ�
struct ScreenGeometricInfo {
  ScreenGeometricInfo(const Camera& camera, const float aspectRatio) noexcept;

  const glm::vec3& GetCenterPos() const noexcept;
  const glm::vec3& GetUpleftPos() const noexcept;
  const glm::vec3& GetHorizontVec() const noexcept;
  const glm::vec3& GetVerticalVec() const noexcept;

private:
  glm::vec3 m_centerPos;
  glm::vec3 m_horizontVec;
  glm::vec3 m_verticalVec;
  glm::vec3 m_upleftPos;
};

}// namespace cges