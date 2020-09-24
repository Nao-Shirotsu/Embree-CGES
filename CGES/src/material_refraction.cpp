#include "material_refraction.hpp"

#include "glm/geometric.hpp"

cges::material::Refraction::Refraction(const float ior) noexcept 
 : m_ior(ior){
}

glm::vec3 cges::material::Refraction::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return glm::refract(incomingRayDir, faceNormal, m_ior);
}

glm::vec3 cges::material::Refraction::IntegrandFactor(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const glm::vec3& albedo) const {
  return albedo; // WIP
}
