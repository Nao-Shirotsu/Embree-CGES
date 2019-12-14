#include "gameobject_sphere.hpp"

#include <embree3/rtcore_geometry.h>

namespace cges::gameobject {

cges::gameobject::Sphere::Sphere(const RTCDevice device, const float radius, const char* const textureFilePath)
    : Base(device, RTC_GEOMETRY_TYPE_SPHERE_POINT)
    , m_value{ { 0.0f, 0.0f, 0.0f }, radius }
    , m_texture(textureFilePath) {
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT4,
      &m_value,
      0,
      sizeof(m_value),
      1);
  rtcCommitGeometry(m_rtcGeometry);
}

bool cges::gameobject::Sphere::HasAttribute() {
  return false;
}

ColorRGBA cges::gameobject::Sphere::GetColor(const float u, const float v) const {
  return { 0xff, 0x40, 0x40, 0xff }; // TODO:ŽÀ‘•‚·‚é
}

}// namespace cges::gameobject

std::unique_ptr<cges::gameobject::Sphere> cges::MakeSphere(const RTCDevice device, const float radius, const char* const textureFilePath) {
  return std::make_unique<gameobject::Sphere>(device, radius, textureFilePath);
}