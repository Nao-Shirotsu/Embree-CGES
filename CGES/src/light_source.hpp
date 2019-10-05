#pragma once

#include <glm/vec3.hpp>

#include "color.hpp"

namespace cges {

// “_ŒõŒ¹ Œ“ wŒü«ŒõŒ¹
struct LightSource {
  union {
    glm::vec3 pos;
    glm::vec3 dir;
  };
  ColorRGBA emitColor;
};

} // namespace cges