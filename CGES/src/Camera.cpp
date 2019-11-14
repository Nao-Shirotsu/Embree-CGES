#include "camera.hpp"

#include <cmath>

#ifdef _DEBUG
#include <iostream>
#endif

cges::Camera::Camera(const glm::vec3& posWorld, const float radius) 
  : posWorld{ posWorld }
  , radius{ radius } {
}

void cges::Camera::UpdatePosLocal() {
  const float sinYZ = std::sinf(radYZ);
  posLocal.x = radius * sinYZ * std::cosf(radXZ);
  posLocal.y = radius * std::cosf(radYZ);
  posLocal.z = radius * sinYZ * std::sinf(radXZ);

#ifdef _DEBUG
  std::cout << "radXZ : ƒÎ*" << (radXZ / 3.14159265358979323846)  << std::endl;
  std::cout << "radYZ : ƒÎ*" << (radYZ / 3.14159265358979323846) << std::endl;
  std::cout << "posLocal : (" << posLocal.x << ", " << posLocal.y << ", " << posLocal.z << ")\n"<< std::endl;
#endif
}

glm::vec3 cges::Camera::GetPosLocal() const{
  return posLocal;
}
