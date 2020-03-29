#include "material_lambertian.hpp"

#include "util_renderer.hpp"

#include <cmath>
#include <glm/geometric.hpp>


namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f; // ColorRGBA‚ð0.0~1.0‚É³‹K‰»‚·‚é‚½‚ß255‚ÅŠ„‚é

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

  return glm::normalize(axisU * (rnd() - 0.5f) +
                        axisV * (rnd() - 0.5f) +
                        axisW * (rnd() * 0.5f));
}

}// anonymous namespace

namespace cges::material {

glm::vec3 Lambertian::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return SampleHemisphericalVec(faceNormal);
}

glm::vec3 Lambertian::BRDF(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const ColorRGBA albedo) const {
  return {
    static_cast<float>(albedo.r) / DENOM / PI,
    static_cast<float>(albedo.g) / DENOM / PI,
    static_cast<float>(albedo.b) / DENOM / PI
  };
}

} // namespace cges::material