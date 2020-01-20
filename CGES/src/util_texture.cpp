#include "util_texture.hpp"
#include "tex_geterrorimage.hpp"
#include "tex_loadppm.hpp"
#include "tex_loadpng.hpp"

#include <string>

namespace {

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

}// anonymous namespace

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