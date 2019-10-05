#pragma once

#include <cstdint>
#include <vector>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace cges {

struct Vertex3f {
  float x, y, z;
};

struct PolygonIndices {
  unsigned int v0, v1, v2;
};

// �V�[���ɔz�u�����C�ӂ�3D���f���̃N���X
class GameObject {
public:
  GameObject(const RTCDevice device); // ��̃I�u�W�F�N�g
  GameObject(const RTCDevice device, const char* const filePath); // 3D���f���Ǎ�
  GameObject(const GameObject& other);

  GameObject(GameObject&& other) noexcept;

  ~GameObject() noexcept;

  GameObject& operator=(GameObject&& other) noexcept;
  GameObject& operator=(const GameObject& other);

  // ���[�h������true, ���s��false��Ԃ�
  bool LoadObjFile(const char* const objFileName);
  uint32_t AttachTo(const RTCScene scene);

private:
  //glm::vec3 origin; // ���f�����W�n���_�̃��[���h�ʒu
  const RTCDevice m_device;
  RTCGeometry m_geometry;
  std::vector<Vertex3f> m_verBuf;
  std::vector<PolygonIndices> m_idxBuf;
};

} // namespace cges