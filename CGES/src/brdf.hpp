#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::brdf {

using BRDFType = glm::vec3(const glm::vec3& surfacePoint,
                       const glm::vec3& outgoingDir,
                       const glm::vec3& incomingDir,
                       const glm::vec3& normal,
                       const ColorRGBA albedo);

glm::vec3 Lambertian(const glm::vec3& surfacePoint,
                     const glm::vec3& outgoingDir,
                     const glm::vec3& incomingDir,
                     const glm::vec3& normal,
                     const ColorRGBA albedo) noexcept;
}