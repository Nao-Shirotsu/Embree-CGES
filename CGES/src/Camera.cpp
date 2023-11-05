#include "camera.hpp"

#include <cmath>
#include <glm/glm.hpp>

namespace {

constexpr float PI = 3.14159265358979323846f;

}

cges::Camera::Camera(const glm::vec3& posWorld, const float radius, const float fov) 
  : posWorld{ posWorld }
  , radius{ radius }
  , radXZ{ PI * 0.5f }
  , radYZ{ PI * 0.5f }
  , fov{ fov }
  , m_justMoved(false){
  UpdatePosLocal();
}

void cges::Camera::UpdatePosLocal() {
  const float sinYZ = std::sinf(radYZ);
  m_posLocal.x = radius * sinYZ * std::cosf(radXZ);
  m_posLocal.y = radius * std::cosf(radYZ);
  m_posLocal.z = radius * sinYZ * std::sinf(radXZ);

  m_justMoved = true;
}

void cges::Camera::StayPosition() {
  m_justMoved = false;
}

glm::vec3 cges::Camera::GetPosLocal() const{
  return m_posLocal;
}

bool cges::Camera::JustMoved() const {
  return m_justMoved;
}

glm::vec3 cges::Camera::GetFrontDir() const noexcept {
  return glm::normalize(glm::vec3{ 0, 0, 0 } - GetPosLocal());
}
