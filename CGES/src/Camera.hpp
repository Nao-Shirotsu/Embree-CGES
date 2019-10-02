#pragma once

#include <glm/vec3.hpp>

namespace cges {

struct Camera {
  glm::vec3 pos = {0.0, 0.0, -5.0};
  glm::vec3 upward = {0.0, 1.0, 0.0};
  glm::vec3 lookingPos = {0.0, 0.0, 0.0};
  float fov = 90.0;
};

}// namespace cges