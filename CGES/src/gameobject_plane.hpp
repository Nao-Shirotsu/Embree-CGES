#pragma once

#include "gameobject_base.hpp"
#include "texture.hpp"

#include <memory>
#include <array>
#include <glm/vec3.hpp>

namespace cges::gameobject {

// �V�[���ɔz�u�����C�ӂ�3D���f���̃N���X
class Plane : public Base {
public:
  // vertex1, vertex2, vertex3��3���_�Ƃ�����`�𐶐� (v1, v2,v3�̏��Ŕ����v���)
  Plane(const RTCDevice device, 
        const glm::vec3& posWorld, 
        const glm::vec3& vertex1, 
        const glm::vec3& vertex2, 
        const glm::vec3& vertex3, 
        const ColorRGBA diffuseColor,
        const ColorRGBA emissionColor, 
        const material::Base& material);

  Plane(const RTCDevice device, 
        const glm::vec3& posWorld, 
        const glm::vec3& vertex1, 
        const glm::vec3& vertex2, 
        const glm::vec3& vertex3, 
        const char* const textureFilePath,
        const ColorRGBA emissionColor,
        const material::Base& material);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

  glm::vec3 GetPosWorld() const override;

private:
  std::array<glm::vec3, 4> m_vertexBuf;
  const std::array<unsigned int, 4> m_indexBuf;
  const glm::vec3 m_posWorld; // ���[���h���W�n�ɂ�����ʒu
  Texture m_texture;
};

} // namespace cges::gameobject

namespace cges {
// vertex1, vertex2, vertex3��3���_�Ƃ�����`�𐶐� (v1, v2, v3�̏��Ŕ����v���)
std::unique_ptr<gameobject::Plane> MakePlane(const RTCDevice device, 
                                             const glm::vec3& posWorld,
                                             const glm::vec3& vertex1, 
                                             const glm::vec3& vertex2, 
                                             const glm::vec3& vertex3, 
                                             const ColorRGBA diffuseColor,
                                             const ColorRGBA emissionColor,
                                             const material::Base& material);

std::unique_ptr<gameobject::Plane> MakePlane(const RTCDevice device,
                                             const glm::vec3& posWorld,
                                             const glm::vec3& vertex1,
                                             const glm::vec3& vertex2,
                                             const glm::vec3& vertex3,
                                             const char* const textureFilePath,
                                             const ColorRGBA emissionColor, 
                                             const material::Base& material);

} // namespace cges