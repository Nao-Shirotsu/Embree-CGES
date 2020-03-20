#pragma once

#include "material_base.hpp"

namespace cges::material {

class Lambertian : public Base {
  glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal,
                                const glm::vec3& incomingRayDir) const override;

  glm::vec3 BRDF(const glm::vec3& surfacePoint,
                 const glm::vec3& outgoingDir,
                 const glm::vec3& incomingDir,
                 const glm::vec3& normal,
                 const ColorRGBA albedo) const override;
};

}