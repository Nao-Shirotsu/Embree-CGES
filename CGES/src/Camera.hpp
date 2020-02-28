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
  float radius = 5.0f; // ���_�Ƃ̋���
  float radXZ = 1.0f;  // x������z�����̊Ԃ̂Ȃ��p
  float radYZ = 1.0f;  // y������z�����̊Ԃ̂Ȃ��p
  float fov;           // ����p

private:
  glm::vec3 m_posLocal;
};

}// namespace cges