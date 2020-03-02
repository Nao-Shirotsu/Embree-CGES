#include "texture.hpp"
#include "util_texture.hpp"

#include <algorithm>

namespace cges {

Texture::Texture(const char* const filePath)
    : m_imageBuffer(tex::LoadFromFile(filePath)) {}

Texture::Texture(const ColorRGBA singleColor)
    : m_imageBuffer(1, 1) {
  m_imageBuffer[0][0] = singleColor;
}

ColorRGBA Texture::GetPixel(const float x, const float y) const noexcept{
  uint32_t xIdx = std::clamp(x, 0.0f, 1.0f) * static_cast<uint32_t>(m_imageBuffer.GetWidth());
  uint32_t yIdx = std::clamp(y, 0.0f, 1.0f) * static_cast<uint32_t>(m_imageBuffer.GetHeight());
  return m_imageBuffer[yIdx][xIdx];
}

#ifdef _DEBUG
void Texture::SaveAsPpm(const char* const filePath) {
  m_imageBuffer.SaveAsPpm(filePath);
}
#endif

}// namespace cges