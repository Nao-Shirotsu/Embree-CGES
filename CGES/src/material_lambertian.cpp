#include "material_lambertian.hpp"

#include "util_renderer.hpp"

#include <cmath>
#include <glm/geometric.hpp>


namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f; // ColorRGBAを0.0~1.0に正規化するため255で割る

cges::RandomGenerator rnd;

glm::vec3 SampleHemisphericalVec(const glm::vec3& axisW) noexcept {
  // 半球の正規直交基底
  glm::vec3 axisU;

  if(std::fabsf(axisW.x) > EPSILON) {
    axisU = glm::normalize(glm::cross({ 0.0f, 1.0f, 0.0f }, axisW));
  }
  else {
    axisU = glm::normalize(glm::cross({ 1.0f, 0.0f, 0.0f }, axisW));
  }
  glm::vec3 axisV = glm::normalize(glm::cross(axisW, axisU));

  const float theta = 2.0f * PI * rnd(); // xz平面のラジアン
  const float phi   = PI * rnd(); // xy平面のラジアン sinで正になるように[0, PI]
  
  return glm::normalize(axisU * std::cosf(theta) +
                        axisV * std::sinf(theta) +
                        axisW * std::sinf(phi));
}

}// anonymous namespace

namespace cges::material {

glm::vec3 Lambertian::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  //if (glm::dot(faceNormal, incomingRayDir) < 0.0f) {
    return SampleHemisphericalVec(faceNormal);
  //}
  //return SampleHemisphericalVec(-faceNormal);
}

glm::vec3 Lambertian::BRDF(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const ColorRGBA albedo) const {
  return {
    static_cast<float>(albedo.r) / DENOM / PI,
    static_cast<float>(albedo.g) / DENOM / PI,
    static_cast<float>(albedo.b) / DENOM / PI
  };
}

} // namespace cges::material