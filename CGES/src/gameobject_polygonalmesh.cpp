#include "gameobject_polygonalmesh.hpp"
#include "util_objfile.hpp"

#include <embree3/rtcore_geometry.h>

#ifdef _DEBUG
#include <cassert>
#endif

namespace cges::gameobject {

PolygonalMesh::PolygonalMesh(const RTCDevice device, const char* const filePath) 
    : Base(device, RTC_GEOMETRY_TYPE_TRIANGLE) {
#ifdef _DEBUG
  assert(obj::LoadFromFile(filePath, m_vertexBuf, m_indexBuf, m_vertexNormalBuf));
#else
  obj::LoadFromFile(filePath, vertexBuf, indexBuf, vertexNormalBuf);
#endif
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      m_vertexBuf.data(),
      0,
      sizeof(Vector3f),
      m_vertexBuf.size());

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT3,
      m_indexBuf.data(),
      0,
      sizeof(PolygonIndex),
      m_indexBuf.size());

  if (HasAttribute()) {
    rtcSetGeometryVertexAttributeCount(m_rtcGeometry, 1);
    rtcSetSharedGeometryBuffer(
        m_rtcGeometry,
        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
        0,
        RTC_FORMAT_FLOAT3,
        m_vertexNormalBuf.data(),
        0,
        sizeof(Vector3f),
        m_vertexNormalBuf.size());
  }

  rtcCommitGeometry(m_rtcGeometry);
}

bool PolygonalMesh::HasAttribute() {
  return m_vertexNormalBuf.size() != 0;
}

} // namespace cges::gameobject

std::unique_ptr<cges::gameobject::PolygonalMesh> cges::MakePolygonalMesh(const RTCDevice device, const char* const filePath) {
  return std::make_unique<gameobject::PolygonalMesh>(device, filePath);
}

