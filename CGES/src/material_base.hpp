#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::material {

class Base {
public:
  inline virtual ~Base(){};

  // RGB‚»‚ê‚¼‚ê‚Ìweight‚ð•Ô‚·
  virtual glm::vec3 BRDF(const glm::vec3& surfacePoint, const glm::vec3& outgoingDir, const glm::vec3& incomingDir, const glm::vec3& normal, const ColorRGBA albedo) const noexcept = 0;
};

}// namespace cges::material