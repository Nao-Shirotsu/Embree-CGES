#include "gameobject_emittable.hpp"

namespace cges::gameobject {

Emittable::Emittable(const ColorRGBA emissionColor)
    : m_emissionColor(emissionColor) {}

bool Emittable::IsEmitting() const noexcept{
  return m_emissionColor.r > 0 || m_emissionColor.g > 0 || m_emissionColor.b > 0;
}

ColorRGBA Emittable::GetEmission() const noexcept {
  return m_emissionColor;
}



} // namespace cges::gameobject