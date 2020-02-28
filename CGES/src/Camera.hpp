#pragma once

#include <glm/vec3.hpp>

namespace cges {

class Camera {
public:
  Camera(const glm::vec3& posWorld, const float radius, const float fov);

  void UpdatePosLocal();
  glm::vec3 GetPosLocal() const;

  glm::vec3 posWorld = { 0.0f, 0.0f, 0.0f };
  glm::vec3 upwardWorld = { 0.0f, 1.0f, 0.0f };
  float radius = 5.0f; // 原点との距離
  float radXZ = 0.0f;  // x成分とz成分の間のなす角
  float radYZ = 0.0f;  // y成分とz成分の間のなす角
  float fov;           // 視野角

private:
  glm::vec3 posLocal = { 0.0f, 0.0f, 0.0f };
};

}// namespace cges