#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <vector>
#include <memory>

namespace cges::gameobject {

// シーンに配置される任意の3Dモデルのクラス
class PolygonalMesh : public Base {
public:
  PolygonalMesh(const RTCDevice device, const char* const filePath);

  bool HasAttribute() override;

  ColorRGBA GetColorByUV(const float x, const float y) override;

private:
  std::vector<Vector3f> m_vertexBuf;
  std::vector<PolygonIndex> m_indexBuf;
  std::vector<Vector3f> m_vertexNormalBuf;
};

} // namespace cges::gameobject

namespace cges{

std::unique_ptr<gameobject::PolygonalMesh> MakePolygonalMesh(const RTCDevice device, const char* const filePath);

} // namespace cges