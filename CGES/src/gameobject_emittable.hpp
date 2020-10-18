#pragma once

#include "color.hpp"

#include <glm/vec3.hpp>

namespace cges::gameobject {

class Emittable {
public:
  Emittable(const ColorRGBA emissionColor);

  bool IsEmitting() const noexcept;

  ColorRGBA GetEmission() const noexcept;

  virtual glm::vec3 SampleSurfacePoint() const noexcept = 0;

private:
  const ColorRGBA m_emissionColor;
};

} // namespace cges::gameobject