#pragma once

#include <cstdint>
#include <glm/vec3.hpp>

namespace cges {

// •’Ê‚ÌFƒNƒ‰ƒX
struct ColorRGBA {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 0;
};

using ColorRGBf = glm::vec3;

} // namespace cges