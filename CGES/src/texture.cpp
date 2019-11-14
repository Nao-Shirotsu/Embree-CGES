#include "texture.hpp"

#include <algorithm>

namespace cges {

Texture::Texture(const size_t width, const size_t height) 
: RenderBuffer(width, height) {}

bool Texture::LoadPpm(const char* const filePath) { // ‚±‚±
  return false;
}

ColorRGBA Texture::GetPixel(const float x, const float y) {
  uint32_t xIdx = std::clamp(x, 0.0f, 1.0f) * GetWidth();
  uint32_t yIdx = std::clamp(y, 0.0f, 1.0f) * GetHeight();
  return (*this)[yIdx][xIdx];
}

}// namespace cges