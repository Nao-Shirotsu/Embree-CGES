#pragma once

#include "renderbuffer.hpp"
#include "Camera.hpp"

#include <glm/vec3.hpp>

namespace cges {

// �`�擊�e�X�N���[��(RenderBuffer)��3D��ԏ�̈ʒu�Ȃ�
class ScreenGeometricInfo {
public:
  ScreenGeometricInfo(const Camera& camera, const RenderBuffer& renderTarget);

  // RenderBuffer[y][x]�̌��A���v�Z����direction���擾
  glm::vec3 ComputeDirection(const unsigned int y, const unsigned int x) const noexcept;
  glm::vec3 ComputePixelPos(const unsigned int y, const unsigned int x) const noexcept;

private:

  const Camera& ref_camera;
  const RenderBuffer& ref_renderTarget;
  const float m_aspectRatio;
  const glm::vec3 m_horizontVec;
  const glm::vec3 m_verticalVec;
  const float m_distanceCamera2Screen;
  const glm::vec3 m_centerPos;
  const glm::vec3 m_upleftPos;
};

} // namespace cges