#include "gameobject_base.hpp"

namespace cges::gameobject{

Base::Base(const RTCDevice device, const RTCGeometryType geomType, const ColorRGBA emissionColor, const material::Base& material) 
    : Emittable(emissionColor)
    , m_rtcDevice(device)
    , m_rtcGeometry(rtcNewGeometry(device, geomType))
    , m_material(material) {}

Base::~Base() noexcept {
  rtcReleaseGeometry(m_rtcGeometry);
}

glm::vec3 Base::ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const {
  return m_material.ComputeReflectedDir(faceNormal, incomingRayDir);
}

glm::vec3 Base::IntegrandFactor(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const glm::vec3& albedo) const {
  return m_material.IntegrandFactor(surfacePoint,
                              outgoingDir,
                              incomingDir,
                              normal,
                              albedo);
}

void Base::AttachTo(const RTCScene scene, const unsigned int geomID) {
  rtcAttachGeometryByID(scene, m_rtcGeometry, geomID);
}

} // namespace cges::gameobject