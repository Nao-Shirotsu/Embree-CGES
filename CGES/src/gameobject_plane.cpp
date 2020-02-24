#include "gameobject_plane.hpp"

#include <embree3/rtcore_geometry.h>
#include <glm/glm.hpp>

namespace {

constexpr size_t NUM_POLYGONS = 2;

}

namespace cges::gameobject {

Plane::Plane(const RTCDevice device, const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& normal, const ColorRGBA diffuseColor)
    : Base(device, RTC_GEOMETRY_TYPE_QUAD)
    , m_diffuseColor(diffuseColor) {
  const auto planeCenterPos = (vertex1 + vertex2) * 0.5f;
  const auto lowerRightDiagDir = (vertex2 - vertex1) * 0.5f;
  const auto normalizedFaceDir = glm::normalize(normal);
  const auto normalizedlowerRightDiagDir = glm::normalize(lowerRightDiagDir);
  const auto normalizedlowerLeftDiagDir = glm::cross(normalizedFaceDir,normalizedlowerRightDiagDir);
  const auto upperRightDiagDir = normalizedlowerLeftDiagDir * glm::length(lowerRightDiagDir);
  
  m_vertexBuf[0] = vertex1;
  m_vertexBuf[1] = planeCenterPos + upperRightDiagDir;
  m_vertexBuf[2] = planeCenterPos - upperRightDiagDir;
  m_vertexBuf[3] = vertex2;

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      &(m_vertexBuf[0]), // 0, 1, 2”Ô–Ú‚Ì’¸“_‚Åƒ|ƒŠƒSƒ“‚ð’£‚é
      0,
      sizeof(decltype(m_vertexBuf[0])),
      1);

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      &(m_vertexBuf[1]), // 1, 2, 3”Ô–Ú‚Ì’¸“_‚Åƒ|ƒŠƒSƒ“‚ð’£‚é
      0,
      sizeof(decltype(m_vertexBuf[0])),
      1);

  rtcCommitGeometry(m_rtcGeometry);
}

bool Plane::HasAttribute() {
  return false;
}

ColorRGBA Plane::GetColor(const float u, const float v) const {
  return m_diffuseColor;
}

RTCGeometryType Plane::GetGeomType() const {
  return RTC_GEOMETRY_TYPE_QUAD;
}

}// namespace cges::gameobject

std::unique_ptr<cges::gameobject::Plane> cges::MakePlane(const RTCDevice device, 
                                                         const glm::vec3& vertex1, 
                                                         const glm::vec3& vertex2, 
                                                         const glm::vec3& normal, 
                                                         const cges::ColorRGBA diffuseColor) {
  return std::make_unique<cges::gameobject::Plane>(device, vertex1, vertex2, normal, diffuseColor);
}