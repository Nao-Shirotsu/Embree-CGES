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
  // �Ƃ肠����.obj(v��f�̂�)����̓ǂݍ��݂�����������
  GameObject(const RTCDevice device);
  ~GameObject();

  // ���[�h������true, ���s��false��Ԃ�
  bool LoadObjFile(const char* const objFileName);
  uint32_t AttachTo(const RTCScene scene);

private:
  //glm::vec3 origin; // ���f�����W�n���_
  RTCGeometry m_geometry;
  std::vector<Vertex3f> m_verBuf;
  std::vector<PolygonIndices> m_idxBuf;
};

} // namespace cges