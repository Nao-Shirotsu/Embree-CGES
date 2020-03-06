#include "gameobject_plane.hpp"
#include "bufferelements.hpp"

#include <embree3/rtcore_geometry.h>
#include <glm/glm.hpp>

namespace {

constexpr size_t NUM_VERTICES = 4;
constexpr size_t NUM_SLOTS = 0;
constexpr size_t NUM_OFFSETS = 0;

}

namespace cges::gameobject {

Plane::Plane(const RTCDevice device, 
             const glm::vec3& posWorld, 
             const glm::vec3& vertex1, 
             const glm::vec3& vertex2, 
             const glm::vec3& vertex3, 
             const ColorRGBA diffuseColor)
    : Base(device, RTC_GEOMETRY_TYPE_QUAD)
    , m_indexBuf{0, 1, 2, 3}
    , m_posWorld(posWorld)
    , m_texture(diffuseColor) {
  const auto v2tov3 = vertex3 - vertex2;
  
  m_vertexBuf[0] = vertex1 + v2tov3 + m_posWorld;
  m_vertexBuf[1] = vertex1 + m_posWorld;
  m_vertexBuf[2] = vertex2 + m_posWorld;
  m_vertexBuf[3] = vertex3 + m_posWorld;

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      NUM_SLOTS,
      RTC_FORMAT_FLOAT3,
      &(m_vertexBuf[0]),
      NUM_OFFSETS,
      sizeof(decltype(m_vertexBuf[0])),
      NUM_VERTICES);

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_INDEX,
      NUM_SLOTS,
      RTC_FORMAT_UINT4,
      &(m_indexBuf[0]),
      NUM_OFFSETS,
      sizeof(decltype(m_indexBuf[0])) * m_indexBuf.size(),
      1);

  rtcCommitGeometry(m_rtcGeometry);
}

Plane::Plane(const RTCDevice device, 
             const glm::vec3& posWorld, 
             const glm::vec3& vertex1, 
             const glm::vec3& vertex2, 
             const glm::vec3& vertex3, 
             const char* const textureFilePath)
    : Base(device, RTC_GEOMETRY_TYPE_QUAD)
    , m_indexBuf{ 0, 1, 2, 3 }
    , m_posWorld(posWorld)
    , m_texture(textureFilePath) {
  const auto v2tov3 = vertex3 - vertex2;

  m_vertexBuf[0] = vertex1 + v2tov3 + m_posWorld;
  m_vertexBuf[1] = vertex1 + m_posWorld;
  m_vertexBuf[2] = vertex2 + m_posWorld;
  m_vertexBuf[3] = vertex3 + m_posWorld;

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      NUM_SLOTS,
      RTC_FORMAT_FLOAT3,
      &(m_vertexBuf[0]),
      NUM_OFFSETS,
      sizeof(decltype(m_vertexBuf[0])),
      NUM_VERTICES);

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_INDEX,
      NUM_SLOTS,
      RTC_FORMAT_UINT4,
      &(m_indexBuf[0]),
      NUM_OFFSETS,
      sizeof(decltype(m_indexBuf[0])) * m_indexBuf.size(),
      1);

  rtcCommitGeometry(m_rtcGeometry);
}

bool Plane::HasAttribute() {
  return false;
}

ColorRGBA Plane::GetColor(const float u, const float v) const {
  return m_texture.GetPixel(u, v);
}

RTCGeometryType Plane::GetGeomType() const {
  return RTC_GEOMETRY_TYPE_QUAD;
}

glm::vec3 Plane::GetPosWorld() const {
  return m_posWorld;
}

}// namespace cges::gameobject

std::unique_ptr<cges::gameobject::Plane> cges::MakePlane(const RTCDevice device, 
                                                         const glm::vec3& posWorld,
                                                         const glm::vec3& vertex1, 
                                                         const glm::vec3& vertex2, 
                                                         const glm::vec3& vertex3, 
                                                         const cges::ColorRGBA diffuseColor) {
  return std::make_unique<cges::gameobject::Plane>(device, posWorld, vertex1, vertex2, vertex3, diffuseColor);
}

std::unique_ptr<cges::gameobject::Plane> cges::MakePlane(const RTCDevice device, 
                                                         const glm::vec3& posWorld, 
                                                         const glm::vec3& vertex1, 
                                                         const glm::vec3& vertex2, 
                                                         const glm::vec3& vertex3, 
                                                         const char* const textureFilePath) {
  return std::make_unique<cges::gameobject::Plane>(device, posWorld, vertex1, vertex2, vertex3, textureFilePath);
}
