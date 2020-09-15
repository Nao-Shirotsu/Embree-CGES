#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::material {

class Base {
public:
  virtual glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal, 
                                     const glm::vec3& incomingRayDir) const = 0;

  // �����_�����O����������radiance�ɂ�����W��(�C���|�[�^���X�T���v�����O���l���������ό`��̌`)
  virtual glm::vec3 IntegrandFactor(const glm::vec3& surfacePoint,
                              const glm::vec3& outgoingDir,
                              const glm::vec3& incomingDir,
                              const glm::vec3& normal,
                              const glm::vec3& albedo) const = 0;
};

} // namespace cges::material