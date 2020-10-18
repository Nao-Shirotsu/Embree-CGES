#include "gameobject_emittable.hpp"

namespace cges::gameobject {

Emittable::Emittable(const ColorRGBA emissionColor)
    : m_emissionColor(emissionColor) {}

ColorRGBA Emittable::GetEmission() const noexcept {
  return m_emissionColor;
}



} // namespace cges::gameobject