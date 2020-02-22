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

}// namespace cges