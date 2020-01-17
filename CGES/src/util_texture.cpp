#include "util_texture.hpp"

#include <fstream>
#include <string>
#include <functional>

namespace {

cges::RenderBuffer GetErrorImage() {
  auto imageBuf = cges::RenderBuffer(1, 1);
  imageBuf[0][0].r = 255u;
  imageBuf[0][0].b = 255u;
  return std::move(imageBuf);
}

enum class FileFormat {
  UNSUPPORTED,
  PNG,
  PPM,
};

FileFormat DetectFileFormat(const char* const filePath) {
  const std::string filePathStr(filePath);
  const auto fileExtension = filePathStr.substr(filePathStr.find_last_of('.') + 1);
  if (fileExtension == "png" || fileExtension == "PNG") {
    return FileFormat::PNG;
  }
  if (fileExtension == "ppm" || fileExtension == "PPM") {
    return FileFormat::PPM;
  }
  return FileFormat::UNSUPPORTED;
}

cges::RenderBuffer LoadPng(const char* const filePath) {
  return GetErrorImage();
}

cges::RenderBuffer LoadPpm(const char* const filePath) {
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

}// anonimous namespace

namespace cges::tex {

RenderBuffer LoadFromFile(const char* const filePath) {
  switch (DetectFileFormat(filePath)) {
    case FileFormat::PNG: {
      return LoadPng(filePath);
    }
    case FileFormat::PPM: {
      return LoadPpm(filePath);
    }
    default: {
      return GetErrorImage();
    }
  }
}

}// namespace cges::tex