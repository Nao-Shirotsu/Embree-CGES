#pragma once

#include <glm/vec3.hpp>
#include <embree3/rtcore_ray.h>

namespace cges {

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

} // namespace cges