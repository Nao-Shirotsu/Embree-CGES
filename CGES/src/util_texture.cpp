#include "util_texture.hpp"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#include "../stb/stb_image.h"

#ifdef _DEBUG
#include <iostream>
#endif

#define EARLY_RETURN_UNSUPPORTED_FORMAT(errMsg, filePath) \
  PrintErrorMsg((errMsg), (filePath)); \
  return GetErrorImage()

namespace {

constexpr unsigned int MAGICNUMBER_LENGTH = 8;

cges::RenderBuffer GetErrorImage() {
  auto imageBuf = cges::RenderBuffer(1, 1);
  imageBuf[0][0].r = 255u;
  imageBuf[0][0].b = 255u;
  return std::move(imageBuf);
}

void PrintErrorMsg(const char* const errMsg, const char* const filePath) {
#ifdef _DEBUG
  std::cout << "<IMAGE> " << (errMsg) << " : " << (filePath) << std::endl;
#endif
}

bool IsFileFormatSupported(const char* const filePath) {
  const std::string filePathStr(filePath);
  const auto fileExtension = filePathStr.substr(filePathStr.find_last_of('.') + 1);

  return fileExtension == "jpg" || fileExtension == "JPG" || fileExtension == "jpeg" || fileExtension == "JPEG" ||
         fileExtension == "pnm" || fileExtension == "PNM" || 
         fileExtension == "pgm" || fileExtension == "PGM" || 
         fileExtension == "ppm" || fileExtension == "PPM" || 
         fileExtension == "png" || fileExtension == "PNG" || 
         fileExtension == "tga" || fileExtension == "TGA" || 
         fileExtension == "bmp" || fileExtension == "BMP" || 
         fileExtension == "psd" || fileExtension == "PSD" || 
         fileExtension == "gif" || fileExtension == "GIF" || 
         fileExtension == "pic" || fileExtension == "PIC";
}

}// anonymous namespace

namespace cges::tex {

RenderBuffer LoadFromFile(const char* const filePath) {
  if(!IsFileFormatSupported(filePath)) {
    EARLY_RETURN_UNSUPPORTED_FORMAT("file format not supported", filePath);
  }

  unsigned char* pixels;
  int width;
  int height;
  int bytesPerPixel;
  pixels = stbi_load(filePath, &width, &height, &bytesPerPixel, 4);
  if (!pixels) {
    EARLY_RETURN_UNSUPPORTED_FORMAT("image loading failed", filePath);
  }

  auto cgesImage = RenderBuffer(static_cast<size_t>(width), static_cast<size_t>(height));
  for (size_t y = 0; y < cgesImage.GetHeight(); ++y) {
    for (size_t x = 0; x < cgesImage.GetWidth(); ++x) {
      cgesImage[y][x].r = *(pixels + (cgesImage.GetWidth() * y + x + 0));
      cgesImage[y][x].g = *(pixels + (cgesImage.GetWidth() * y + x + 1));
      cgesImage[y][x].b = *(pixels + (cgesImage.GetWidth() * y + x + 2));
      cgesImage[y][x].a = *(pixels + (cgesImage.GetWidth() * y + x + 3));
    }
  }

  // ƒƒ‚ƒŠã‚Ì‰æ‘œƒf[ƒ^‚ð”jŠü
  stbi_image_free(pixels);
  return std::move(cgesImage);
}

}// namespace cges::tex