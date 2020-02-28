#include "camera.hpp"

#include <cmath>

cges::Camera::Camera(const glm::vec3& posWorld, const float radius, const float fov) 
  : posWorld{ posWorld }
  , radius{ radius }
  , fov{ fov } {
  UpdatePosLocal();
}

void cges::Camera::UpdatePosLocal() {
  const float sinYZ = std::sinf(radYZ);
  posLocal.x = radius * sinYZ * std::cosf(radXZ);
  posLocal.y = radius * std::cosf(radYZ);
  posLocal.z = radius * sinYZ * std::sinf(radXZ);
}

glm::vec3 cges::Camera::GetPosLocal() const{
  return posLocal;
}
