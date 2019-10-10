#pragma once

#include <glm/vec3.hpp>

namespace cges {

enum class RenderMode {
  NORMAL,
  UV,
  PHONG_DIFFUSE,
  PHONG_SPECULAR,
  PHONG_AMBIENT,
  PHONG_SHADING
};

class Camera {
public:
  Camera(const glm::vec3& posWorld, const float radius);

  void UpdatePosLocal();
  glm::vec3 GetPosLocal() const;

  glm::vec3 posWorld = { 0.0f, 0.0f, 0.0f };
  glm::vec3 upwardWorld = { 0.0f, 1.0f, 0.0f };
  float radius = 5.0f;
  float radXZ = 0.0f; // x成分とz成分の間のなす角
  float radYZ = 0.0f; // y成分とz成分の間のなす角
  RenderMode mode = RenderMode::PHONG_SHADING;

private:
  //glm::vec3 lookingPosLocal = { 0.0, 0.0, 0.0 };
  glm::vec3 posLocal = { 0.0f, 0.0f, 0.0f };
};

}// namespace cges