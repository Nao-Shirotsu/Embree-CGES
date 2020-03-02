#include "gameobject_sphere.hpp"

#include <embree3/rtcore_geometry.h>

#include <cmath>
#include <iostream>

#ifdef _DEBUG
#include <cassert>
#endif

namespace cges::gameobject {

Sphere::Sphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const char* const textureFilePath)
    : Base(device, RTC_GEOMETRY_TYPE_SPHERE_POINT)
    , m_posWorld(posWorld)
    , m_radius(radius)
    , m_texture(textureFilePath) {
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT4,
      &m_posWorld,
      0,
      sizeof(m_posWorld),
      1);

  rtcCommitGeometry(m_rtcGeometry);
}

Sphere::Sphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const ColorRGBA diffuseColor)
    : Base(device, RTC_GEOMETRY_TYPE_SPHERE_POINT)
    , m_posWorld(posWorld)
    , m_radius(radius)
    , m_texture(diffuseColor) {
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT4,
      &m_posWorld,
      0,
      sizeof(m_posWorld),
      1);

  rtcCommitGeometry(m_rtcGeometry);
}

bool Sphere::HasAttribute() {
  return false;
}

ColorRGBA Sphere::GetColor(const float u, const float v) const {
  return m_texture.GetPixel(u, v);
}

RTCGeometryType Sphere::GetGeomType() const {
  return RTC_GEOMETRY_TYPE_SPHERE_POINT;
}

glm::vec3 Sphere::GetPosWorld() const {
  return m_posWorld;
}

}// namespace cges::gameobject

std::unique_ptr<cges::gameobject::Sphere> cges::MakeSphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const char* const textureFilePath) {
  return std::make_unique<gameobject::Sphere>(device, posWorld, radius, textureFilePath);
}

std::unique_ptr<cges::gameobject::Sphere> cges::MakeSphere(const RTCDevice device, const glm::vec3& posWorld, const float radius, const ColorRGBA diffuseColor) {
  return std::make_unique<gameobject::Sphere>(device, posWorld, radius, diffuseColor);
}
