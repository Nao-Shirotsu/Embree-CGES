#pragma once

#include <glm/vec3.hpp>

namespace cges {

struct Camera {
  glm::vec3 pos;
  glm::vec3 forward;
  glm::vec3 upward;
};

}// namespace cges