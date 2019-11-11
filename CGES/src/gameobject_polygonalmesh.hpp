#pragma once

#include "gameobject_base.hpp"
#include "bufferelements.hpp"

#include <vector>

namespace cges::gameobject {

// �V�[���ɔz�u�����C�ӂ�3D���f���̃N���X
class PolygonalMesh : public Base {
public:
  PolygonalMesh(const RTCDevice device, std::vector<Vector3f> vertexBuf, std::vector<PolygonIndex> indexBuf, std::vector<Vector3f> vertexNormalBuf);

  ~PolygonalMesh() noexcept;

  bool HasAttribute() override;

private:
  std::vector<Vector3f> m_vertexBuf;
  std::vector<PolygonIndex> m_indexBuf;
  std::vector<Vector3f> m_vertexNormalBuf;
};

} // namespace cges::gameobject