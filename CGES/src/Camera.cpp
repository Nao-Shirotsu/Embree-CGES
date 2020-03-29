#include "camera.hpp"

#include <cmath>

namespace {

constexpr float PI = 3.14159265358979323846f;

}

cges::Camera::Camera(const glm::vec3& posWorld, const float radius, const float fov) 
  : posWorld{ posWorld }
  , radius{ radius }
  , radXZ{ PI * 0.5f }
  , radYZ{ PI * 0.5f }
  , fov{ fov } {
  UpdatePosLocal();
}

void cges::Camera::UpdatePosLocal() {
  const float sinYZ = std::sinf(radYZ);
  m_posLocal.x = radius * sinYZ * std::cosf(radXZ);
  m_posLocal.y = radius * std::cosf(radYZ);
  m_posLocal.z = radius * sinYZ * std::sinf(radXZ);
}

glm::vec3 cges::Camera::GetPosLocal() const{
  return m_posLocal;
}
