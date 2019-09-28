#pragma once

#include <glm/vec3.hpp>

namespace cges {

struct Camera {
  glm::vec3 pos;
  glm::vec3 upward;
  double fov;
};

}// namespace cges