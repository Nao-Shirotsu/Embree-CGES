#include "material_lambertian.hpp"

#include "util_renderer.hpp"

#include <cmath>
#include <glm/geometric.hpp>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f / PI; // ColorRGBA‚ð0.0~1.0‚É³‹K‰»‚·‚é‚½‚ß255‚ÅŠ„‚é

cges::RandomGenerator rnd;

glm::vec3 SampleHemisphericalVec(const glm::vec3& org, const glm::vec3& axisW) noexcept {
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
  const float rootR2 = std::sqrtf(r2);

  return glm::normalize(axisU * std::cosf(r1) * rootR2 + 
                        axisV * std::sinf(r1) * rootR2 +
                        axisW * std::sqrtf(1.0f - r2));
}

}// anonymous namespace

namespace cges::material {

glm::vec3 Lambertian::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return glm::vec3();
}

glm::vec3 Lambertian::BRDF(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const ColorRGBA albedo) const {
  return {
    static_cast<float>(albedo.r) / DENOM,
    static_cast<float>(albedo.g) / DENOM,
    static_cast<float>(albedo.b) / DENOM
  };
}

} // namespace cges::material