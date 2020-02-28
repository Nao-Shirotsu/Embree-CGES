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
  float radius = 5.0f; // Œ´“_‚Æ‚Ì‹——£
  float radXZ = 1.0f;  // x¬•ª‚Æz¬•ª‚ÌŠÔ‚Ì‚È‚·Šp
  float radYZ = 1.0f;  // y¬•ª‚Æz¬•ª‚ÌŠÔ‚Ì‚È‚·Šp
  float fov;           // ‹–ìŠp

private:
  glm::vec3 m_posLocal;
};

}// namespace cges