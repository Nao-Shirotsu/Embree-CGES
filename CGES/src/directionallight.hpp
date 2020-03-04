#pragma once

#include <glm/vec3.hpp>

#include "color.hpp"

namespace cges {

struct DirectionalLight {
  glm::vec3 dir;
  ColorRGBA emitColor;
};

} // namespace cges