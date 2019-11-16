#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <vector>
#include <memory>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class PolygonalMesh : public Base {
public:
  PolygonalMesh(const RTCDevice device, const char* const filePath, ColorRGBA diffuseColor);

  bool HasAttribute() override;

  ColorRGBA GetColorByUV(const float x, const float y) override;

private:
  std::vector<Vector3f> m_vertexBuf;
  std::vector<PolygonIndex> m_indexBuf;
  std::vector<Vector3f> m_vertexNormalBuf;
  ColorRGBA m_diffuseColor;
};

} // namespace cges::gameobject

namespace cges{

std::unique_ptr<gameobject::PolygonalMesh> MakePolygonalMesh(const RTCDevice device, const char* const filePath, ColorRGBA diffuseColor);

} // namespace cges