#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

// �v�f���Ƃ̐�
glm::vec3 operator*(const glm::vec3& vec, const cges::ColorRGBA rgba);

// �v�f���Ƃ̐�
glm::vec3 operator*(const cges::ColorRGBA rgba, const glm::vec3& vec);