#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::material {

class Base {
public:
  virtual glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal, 
                                     const glm::vec3& incomingRayDir) const = 0;

  virtual glm::vec3 BRDF(const glm::vec3& surfacePoint,
                         const glm::vec3& outgoingDir,
                         const glm::vec3& incomingDir,
                         const glm::vec3& normal,
                         const ColorRGBA albedo) const = 0;
};

} // namespace cges::material