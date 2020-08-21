#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::material {

class Base {
public:
  virtual glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal, 
                                     const glm::vec3& incomingRayDir) const = 0;

  // レンダリング方程式中のradianceにかける係数(インポータンスサンプリングを考慮した式変形後の形)
  virtual glm::vec3 IntegrandFactor(const glm::vec3& surfacePoint,
                              const glm::vec3& outgoingDir,
                              const glm::vec3& incomingDir,
                              const glm::vec3& normal,
                              const glm::vec3& albedo) const = 0;
};

} // namespace cges::material