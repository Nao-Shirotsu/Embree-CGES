#pragma once

#include <glm/vec3.hpp>

namespace cges {

class Camera {
public:
  Camera(const glm::vec3& posWorld, const float radius);

  void UpdatePosLocal();
  glm::vec3 GetPosLocal() const;

  glm::vec3 posWorld = { 0.0f, 0.0f, 0.0f };
  glm::vec3 upwardWorld = { 0.0f, 1.0f, 0.0f };
  float radius = 5.0f;
  float radXZ = 0.0f; // x¬•ª‚Æz¬•ª‚ÌŠÔ‚Ì‚È‚·Šp
  float radYZ = 0.0f; // y¬•ª‚Æz¬•ª‚ÌŠÔ‚Ì‚È‚·Šp

private:
  //glm::vec3 lookingPosLocal = { 0.0, 0.0, 0.0 };
  glm::vec3 posLocal = { 0.0f, 0.0f, 0.0f };
};

}// namespace cges