#include "gameobject_base.hpp"

namespace cges::gameobject{

Base::Base(const RTCDevice device, const RTCGeometryType geomType) 
    : m_rtcDevice(device)
    , m_rtcGeometry(rtcNewGeometry(device, geomType)) {}

Base::~Base() noexcept {
  rtcReleaseGeometry(m_rtcGeometry);
}

unsigned int Base::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_rtcGeometry);
}

} // namespace cges::gameobject