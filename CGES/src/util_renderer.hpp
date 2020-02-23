#pragma once

#include <glm/vec3.hpp>
#include <embree3/rtcore_ray.h>
#include <embree3/rtcore_geometry.h>

namespace cges {

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

bool IsInterpolatable(const RTCGeometryType geomType) noexcept;

bool WasIntersected(const unsigned int geomID) noexcept;

} // namespace cges