#pragma once

#include "material_base.hpp"

namespace cges::material {

class Lambertian : public Base {
public:
  glm::vec3 BRDF(const glm::vec3& surfacePoint, 
                 const glm::vec3& outgoingDir, 
                 const glm::vec3& incomingDir, 
                 const glm::vec3& normal, 
                 const ColorRGBA albedo) const noexcept override;
};

} // namespace cges::material