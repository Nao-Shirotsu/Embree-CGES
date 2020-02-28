#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <memory>
#include <array>
#include <glm/vec3.hpp>

namespace cges::gameobject {

// �V�[���ɔz�u�����C�ӂ�3D���f���̃N���X
class Plane : public Base {
public:
  // vertex1, vertex2, vertex3��3���_�Ƃ�����`�𐶐� (v1, v2,v3�̏��Ŕ����v���)
  Plane(const RTCDevice device, const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& vertex3, const ColorRGBA diffuseColor);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

private:
  std::array<glm::vec3, 4> m_vertexBuf;
  const std::array<unsigned int, 4> m_indexBuf;
  ColorRGBA m_diffuseColor;
};

} // namespace cges::gameobject

namespace cges {
// vertex1, vertex2, vertex3��3���_�Ƃ�����`�𐶐� (v1, v2, v3�̏��Ŕ����v���)
std::unique_ptr<gameobject::Plane> MakePlane(const RTCDevice device, 
                                             const glm::vec3& vertex1, 
                                             const glm::vec3& vertex2, 
                                             const glm::vec3& vertex3, 
                                             const ColorRGBA diffuseColor);

} // namespace cges