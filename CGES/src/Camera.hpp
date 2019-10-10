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
  float radXZ = 0.0f; // xê¨ï™Ç∆zê¨ï™ÇÃä‘ÇÃÇ»Ç∑äp
  float radYZ = 0.0f; // yê¨ï™Ç∆zê¨ï™ÇÃä‘ÇÃÇ»Ç∑äp
  RenderMode mode = RenderMode::PHONG_SHADING;

private:
  //glm::vec3 lookingPosLocal = { 0.0, 0.0, 0.0 };
  glm::vec3 posLocal = { 0.0f, 0.0f, 0.0f };
};

}// namespace cges