#include "texture.hpp"

#include <algorithm>
#include <fstream>
#include <cassert>

namespace {

cges::RenderBuffer GetErrorImage() {
  auto imageBuf = cges::RenderBuffer(50, 50);
  for (auto y = 0; y < imageBuf.GetHeight(); ++y) {
    for(auto x = 0; x < imageBuf.GetWidth(); ++x) {
      imageBuf[y][x].r = 255u;
      imageBuf[y][x].b = 255u;
    }
  }
  return std::move(imageBuf);
}

cges::RenderBuffer LoadFromFile(const char* const filePath) {
  using namespace cges;
  std::ifstream ifs(filePath);
  if(!ifs) {
    return GetErrorImage();
  }


  const char magicNum1 = ifs.get();
  const char magicNum2 = ifs.get();
  if(magicNum1 != 'P' || magicNum2 != '3') {
    return GetErrorImage();
  }
  size_t width;
  size_t height;
  size_t colorNumber;
  ifs >> width >> height >> colorNumber;

  auto imageBuf = RenderBuffer(width, height);

  for(auto y = 0u; y < height; ++y) {
    for (auto x = 0u; x < width; ++x) {
      uint32_t recv;
      ifs >> recv;
      imageBuf[y][x].r = static_cast<uint8_t>(recv);
      ifs >> recv;
      imageBuf[y][x].g = static_cast<uint8_t>(recv);
      ifs >> recv;
      imageBuf[y][x].b = static_cast<uint8_t>(recv);
    }
  }

  return std::move(imageBuf);
}

}

namespace cges {

Texture::Texture(const char* const filePath) 
: m_imageBuffer(LoadFromFile(filePath)) {}

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