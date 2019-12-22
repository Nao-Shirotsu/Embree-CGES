#include "gameobject_base.hpp"

namespace cges::gameobject{

Base::Base(const RTCDevice device, const RTCGeometryType geomType) 
    : m_rtcDevice(device)
    , m_rtcGeometry(rtcNewGeometry(device, geomType)) {}

Base::~Base() noexcept {
  rtcReleaseGeometry(m_rtcGeometry);
}

void Base::AttachTo(const RTCScene scene, const unsigned int geomID) {
  rtcAttachGeometryByID(scene, m_rtcGeometry, geomID);
}

} // namespace cges::gameobject