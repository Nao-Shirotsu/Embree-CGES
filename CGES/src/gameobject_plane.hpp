#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <memory>
#include <array>
#include <glm/vec3.hpp>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class Plane : public Base {
public:
  // vertex1, vertex2を2頂点としnormalに垂直な矩形を生成
  Plane(const RTCDevice device, const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& normal, const ColorRGBA diffuseColor);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

private:
  std::array<glm::vec3, 4> m_vertexBuf;
  glm::vec3 m_normal;
  ColorRGBA m_diffuseColor;
};

} // namespace cges::gameobject

namespace cges {

std::unique_ptr<gameobject::Plane> MakePlane(const RTCDevice device, 
                                             const glm::vec3& vertex1, 
                                             const glm::vec3& vertex2, 
                                             const glm::vec3& normal, 
                                             const ColorRGBA diffuseColor);

} // namespace cges