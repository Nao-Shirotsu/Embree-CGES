#include "gameobject_base.hpp"

namespace cges::gameobject{

Base::Base(const RTCDevice device) 
  : m_rtcDevice(device)
    , m_rtcGeometry(nullptr) {}

Base::~Base() noexcept {}

unsigned int Base::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_rtcGeometry);
}

} // namespace cges::gameobject