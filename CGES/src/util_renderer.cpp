#include "util_renderer.hpp"

#include <limits> // float infinity
#include <embree3/rtcore_scene.h>

namespace cges {

RussianRoulette::RussianRoulette(RandomGenerator& rnd, const size_t lowerLimit, const size_t upperLimit, const float hitRate)
    : m_rnd(rnd)
    , m_numTrials(0)
    , m_lowerRecursionLimit(lowerLimit)
    , m_upperRecursionLimit(upperLimit)
    , m_continuationRate(1.0f - hitRate) {}

bool RussianRoulette::Spin() noexcept {
  ++m_numTrials;
  if (m_numTrials < m_lowerRecursionLimit) {
    return false;
  }
  return m_numTrials >= m_upperRecursionLimit || m_rnd() > m_continuationRate;
}

void RussianRoulette::SetContinueRate(const float rate) noexcept{
  m_continuationRate = rate;
}

float RussianRoulette::ContinueRate() const noexcept {
  if (m_numTrials < m_lowerRecursionLimit) {
    return 1.0f;
  }
  return m_continuationRate;
}

void cges::InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept {
  rayhit.ray.org_x = org.x;
  rayhit.ray.org_y = org.y;
  rayhit.ray.org_z = org.z;
  rayhit.ray.dir_x = dir.x;
  rayhit.ray.dir_y = dir.y;
  rayhit.ray.dir_z = dir.z;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = std::numeric_limits<float>::max();
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
}

bool IsInterpolatable(const RTCGeometryType geomType) noexcept{
  if (geomType == RTC_GEOMETRY_TYPE_GRID || geomType == RTC_GEOMETRY_TYPE_INSTANCE || geomType == RTC_GEOMETRY_TYPE_DISC_POINT || geomType == RTC_GEOMETRY_TYPE_SPHERE_POINT || geomType == RTC_GEOMETRY_TYPE_USER || geomType == RTC_GEOMETRY_TYPE_QUAD) {
    return false;
  }
  return true;
}

bool WasIntersected(const unsigned int geomID) noexcept{
  return geomID != static_cast<int>(-1);
}

WrappedRayHit::WrappedRayHit(const glm::vec3& org, const glm::vec3& dir)
    : rayhit() {
  rayhit.ray.org_x = org.x;
  rayhit.ray.org_y = org.y;
  rayhit.ray.org_z = org.z;
  rayhit.ray.dir_x = dir.x;
  rayhit.ray.dir_y = dir.y;
  rayhit.ray.dir_z = dir.z;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = std::numeric_limits<float>::max();
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
}

bool WrappedRayHit::Intersect(const Scene& scene, RTCIntersectContext& context) {
  rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);
  return WasIntersected(rayhit.hit.geomID);
}

glm::vec3 WrappedRayHit::RayOrigin() const noexcept {
  return glm::vec3(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z);
}

glm::vec3 WrappedRayHit::RayDir() const noexcept {
  return glm::vec3(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
}

glm::vec3 WrappedRayHit::HitNormal() const noexcept {
  return glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
}

float WrappedRayHit::HitU() const noexcept {
  return rayhit.hit.u;
}

float WrappedRayHit::HitV() const noexcept {
  return rayhit.hit.v;
}

float WrappedRayHit::RayTnear() const noexcept {
  return rayhit.ray.tnear;
}

float WrappedRayHit::RayTfar() const noexcept {
  return rayhit.ray.tfar;
}

float WrappedRayHit::RayTime() const noexcept {
  return rayhit.ray.time;
}

unsigned int WrappedRayHit::RayMask() const noexcept {
  return rayhit.ray.mask;
}

unsigned int WrappedRayHit::RayId() const noexcept {
  return rayhit.ray.id;
}

unsigned int WrappedRayHit::RayFlags() const noexcept {
  return rayhit.ray.flags;
}

unsigned int WrappedRayHit::HitPrimId() const noexcept {
  return rayhit.hit.primID;
}

unsigned int WrappedRayHit::HitGeomId() const noexcept {
  return rayhit.hit.geomID;
}

} // namespace cges