#include "material_lambertian.hpp"

#include "util_renderer.hpp"

#include <cmath>
#include <glm/geometric.hpp>


namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.14159265358979323846f;

cges::RandomGenerator rnd;

glm::vec3 SampleHemisphericalVec(const glm::vec3& axisW) noexcept {
  // ”¼‹…‚Ì³‹K’¼ŒðŠî’ê
  glm::vec3 axisU;

  if(std::fabsf(axisW.x) > EPSILON) {
    axisU = glm::normalize(glm::cross({ 0.0f, 1.0f, 0.0f }, axisW));
  }
  else {
    axisU = glm::normalize(glm::cross({ 1.0f, 0.0f, 0.0f }, axisW));
  }
  glm::vec3 axisV = glm::normalize(glm::cross(axisW, axisU));

  const float r1 = 2.0f * PI * rnd();
  const float r2 = rnd();
  const float r2s = std::sqrtf(r2);
  return glm::normalize(
      axisU * std::cosf(r1) * r2s + 
      axisV * std::sinf(r1) * r2s + 
      axisW * std::sqrtf(1.0 - r2)
  );
}

}// anonymous namespace

namespace cges::material {

glm::vec3 Lambertian::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  if (glm::dot(faceNormal, incomingRayDir) < 0.0f) {
    return SampleHemisphericalVec(faceNormal);
  }
  return SampleHemisphericalVec(-faceNormal);
}

glm::vec3 Lambertian::IntegrandFactor(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const glm::vec3& albedo) const {
  return albedo;
}

} // namespace cges::material