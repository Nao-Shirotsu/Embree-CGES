#pragma once

#include "bufferelements.hpp"
#include "gameobject_base.hpp"
#include "texture.hpp"

#include <memory>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class Sphere : public Base {
public:
  Sphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const char* const textureFilePath);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

  glm::vec3 GetPosWorld() const override;

private:
  glm::vec3 m_posWorld;
  float m_radius;
  Texture m_texture;
};

} // namespace cges::gameobject

namespace cges {

std::unique_ptr<gameobject::Sphere> MakeSphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const char* const textureFilePath);

} // namespace cges