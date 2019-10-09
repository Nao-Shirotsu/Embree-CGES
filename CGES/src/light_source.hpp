#pragma once

#include <glm/vec3.hpp>

#include "color.hpp"

namespace cges {

// �_���� �� �w��������
struct LightSource {
  union {
    glm::vec3 pos;
    glm::vec3 dir;
  };
  ColorRGBA emitColor;
};

} // namespace cges