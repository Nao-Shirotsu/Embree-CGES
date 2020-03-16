#include "material_lambertian.hpp"

#include <cmath>

namespace {

constexpr float PI = 3.14159265358979323846f;

}

namespace cges::material {

glm::vec3 Lambertian::BRDF(const glm::vec3& surfacePoint, 
                           const glm::vec3& outgoingDir, 
                           const glm::vec3& incomingDir, 
                           const glm::vec3& normal, 
                           const ColorRGBA albedo) const noexcept {
  return {
    static_cast<float>(albedo.r) / PI,
    static_cast<float>(albedo.g) / PI,
    static_cast<float>(albedo.b) / PI
  };
}

}// namespace cges::material