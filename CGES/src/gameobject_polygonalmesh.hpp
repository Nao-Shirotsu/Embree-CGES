#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <vector>
#include <memory>
#include <glm/vec3.hpp>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class PolygonalMesh : public Base {
public:
  PolygonalMesh(const RTCDevice device, 
                const glm::vec3& posWorld, 
                const char* const filePath, 
                const ColorRGBA diffuseColor, 
                const ColorRGBA emissionColor);

  bool HasAttribute() override;

  ColorRGBA GetColor(const float u, const float v) const override;

  RTCGeometryType GetGeomType() const override;

  glm::vec3 GetPosWorld() const override;

private:
  std::vector<glm::vec3> m_vertexBuf;
  std::vector<PolygonIndex> m_indexBuf;
  std::vector<glm::vec3> m_vertexNormalBuf;
  glm::vec3 m_posWorld;
  ColorRGBA m_diffuseColor;
};

} // namespace cges::gameobject

namespace cges{

std::unique_ptr<gameobject::PolygonalMesh> MakePolygonalMesh(const RTCDevice device, 
                                                             const glm::vec3& posWorld, 
                                                             const char* const filePath, 
                                                             const ColorRGBA diffuseColor, 
                                                             const ColorRGBA emissionColor);

} // namespace cges