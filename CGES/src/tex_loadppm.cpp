#include "tex_loadppm.hpp"
#include "tex_geterrorimage.hpp"

#include <fstream>

namespace cges::tex {

RenderBuffer LoadPpm(const char* const filePath) {
  using namespace cges;
  std::ifstream ifs(filePath);
  if (!ifs) {
    return GetErrorImage();
  }

  const char magicNum1 = ifs.get();
  const char magicNum2 = ifs.get();
  if (magicNum1 != 'P' || magicNum2 != '3') {
    return GetErrorImage();
  }
  size_t width;
  size_t height;
  size_t colorNumber;
  ifs >> width >> height >> colorNumber;

  auto imageBuf = RenderBuffer(width, height);

  for (auto y = 0u; y < height; ++y) {
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

} // namespace cges::tex