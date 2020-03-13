#pragma once

#include "color.hpp"

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>
#include <glm/vec3.hpp>

namespace cges::gameobject{

// �V�[���ɔz�u�����C�ӂ�3D���f���̃N���X
class Base {
public:
  Base(const RTCDevice device, const RTCGeometryType geomType, const ColorRGBA emissionColor);

  virtual ~Base() noexcept;

  // ��K�{����(Embree�̃o�b�t�@��slot�Ɋi�[����l)�������Ă��邩�ǂ���
  virtual bool HasAttribute() = 0;

  // �I�u�W�F�N�g�̃e�N�X�`�����W����F�擾
  virtual ColorRGBA GetColor(const float u, const float v) const = 0;

  ColorRGBA GetEmission() const;

  virtual RTCGeometryType GetGeomType() const = 0;

  // ���[���h���W�ʒu���擾 (�����o�ϐ��̃A���C�������g�̓s���ŏ������z)
  virtual glm::vec3 GetPosWorld() const = 0;

  // Embree�Ǘ����̃V�[���ɂ��̃I�u�W�F�N�g��o�^
  void AttachTo(const RTCScene scene, const unsigned int geomID);

protected:
  const RTCDevice m_rtcDevice;
  RTCGeometry m_rtcGeometry;
  const ColorRGBA m_emissionColor;
};

} // namespace cges::gameobject