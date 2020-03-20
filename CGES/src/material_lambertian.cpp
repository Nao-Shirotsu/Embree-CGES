#include "material_lambertian.hpp"

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float DENOM = 255.0f / PI; // ColorRGBA��0.0~1.0�ɐ��K�����邽��255�Ŋ���

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