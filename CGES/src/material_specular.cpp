#include "material_specular.hpp"

#include "glm/geometric.hpp"

glm::vec3 cges::material::Specular::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return glm::reflect(incomingRayDir, faceNormal);
}

glm::vec3 cges::material::Specular::IntegrandFactor(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const glm::vec3& albedo) const {
  return albedo;
}
