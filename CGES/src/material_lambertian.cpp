#include "material_lambertian.hpp"

#include <cmath>
#include <glm/geometric.hpp>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f / PI; // ColorRGBA‚ð0.0~1.0‚É³‹K‰»‚·‚é‚½‚ß255‚ÅŠ„‚é

glm::vec3 SampleHemisphericalVec(const glm::vec3& org, const glm::vec3 normal) noexcept {
  // ”¼‹…‚Ì³‹K’¼ŒðŠî’ê
  glm::vec3 basisX;
  glm::vec3 basisZ = normal;

  if(std::fabsf(basisZ.x) > EPSILON) {
    basisX = glm::normalize(glm::cross(basisZ, { 0.0f, 1.0f, 0.0f }));
  }
  else {
    basisX = glm::normalize(glm::cross(basisZ, { 1.0f, 0.0f, 0.0f }));
  }
  glm::vec3 basisY = glm::normalize(glm::cross(normal, basisX));
}

}// anonymous namespace

namespace cges::material {

glm::vec3 Lambertian::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return ;
}

glm::vec3 Lambertian::BRDF(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const ColorRGBA albedo) const {
  return {
    static_cast<float>(albedo.r) / DENOM,
    static_cast<float>(albedo.g) / DENOM,
    static_cast<float>(albedo.b) / DENOM
  };
}

} // namespace cges::material