#include "operators.hpp"

// params : vec:[0.0f, 1.0f],  rgba:[0, 255]
// return : [0.0f, 1.0f]
glm::vec3 operator*(const glm::vec3& vec, const cges::ColorRGBA rgba) {
  return { vec.r * static_cast<float>(rgba.r) / 255.0f,
           vec.g * static_cast<float>(rgba.g) / 255.0f,
           vec.b * static_cast<float>(rgba.b) / 255.0f };
}

// params : vec:[0.0f, 1.0f],  rgba:[0, 255]
// return : [0.0f, 1.0f]
glm::vec3 operator*(const cges::ColorRGBA rgba, const glm::vec3& vec) {
  return vec * rgba;
}
