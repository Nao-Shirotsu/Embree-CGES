#include "brdf.hpp"

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f / PI; // ColorRGBAÇ0.0~1.0Ç…ê≥ãKâªÇ∑ÇÈÇΩÇﬂ255Ç≈äÑÇÈ

}

glm::vec3 cges::brdf::Lambertian(const glm::vec3& surfacePoint, 
                                 const glm::vec3& outgoingDir, 
                                 const glm::vec3& incomingDir, 
                                 const glm::vec3& normal, 
                                 const ColorRGBA albedo) noexcept {
  return {
    static_cast<float>(albedo.r) / DENOM,
    static_cast<float>(albedo.g) / DENOM,
    static_cast<float>(albedo.b) / DENOM
  };
}
