#include "util_renderer.hpp"

#include <limits> // float infinity

namespace cges {

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
  if (geomType == RTC_GEOMETRY_TYPE_GRID || geomType == RTC_GEOMETRY_TYPE_INSTANCE || geomType == RTC_GEOMETRY_TYPE_DISC_POINT || geomType == RTC_GEOMETRY_TYPE_SPHERE_POINT || geomType == RTC_GEOMETRY_TYPE_USER) {
    return false;
  }
  return true;
}

bool WasIntersected(const unsigned int geomID) noexcept{
  return geomID != static_cast<int>(-1);
}

}// namespace cges