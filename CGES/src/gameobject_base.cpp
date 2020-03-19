#include "gameobject_base.hpp"

namespace cges::gameobject{

Base::Base(const RTCDevice device, const RTCGeometryType geomType, const ColorRGBA emissionColor, brdf::BRDFType* brdf) 
    : BRDF(brdf)
    , m_rtcDevice(device)
    , m_rtcGeometry(rtcNewGeometry(device, geomType))
    , m_emissionColor(emissionColor) {}

Base::~Base() noexcept {
  rtcReleaseGeometry(m_rtcGeometry);
}

ColorRGBA Base::GetEmission() const {
  return m_emissionColor;
}

void Base::AttachTo(const RTCScene scene, const unsigned int geomID) {
  rtcAttachGeometryByID(scene, m_rtcGeometry, geomID);
}

} // namespace cges::gameobject