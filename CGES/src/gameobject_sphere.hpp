#pragma once

#include "bufferelements.hpp"
#include "gameobject_base.hpp"
#include "texture.hpp"

#include <memory>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class Sphere : public Base {
public:
  Sphere(const RTCDevice device, const float radius, const char* const textureFilePath);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

private:
  struct {
    Vector3f m_center;
    float m_radius;
  } m_value;
  Texture m_texture;
};

} // namespace cges::gameobject

namespace cges {

std::unique_ptr<gameobject::Sphere> MakeSphere(const RTCDevice device, const float radius, const char* const textureFilePath);

} // namespace cges