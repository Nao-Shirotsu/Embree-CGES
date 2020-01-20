#include "tex_loadpng.hpp"
#include "tex_geterrorimage.hpp"

#include <png.h>
#include <stdio.h>

#ifdef _DEBUG
#include <iostream>
#endif

#define PNG_EARLY_RETURN(errMsg, filePath) \
  PrintErrorMsg((errMsg), (filePath));     \
  return GetErrorImage()

namespace {

constexpr unsigned int MAGICNUMBER_LENGTH = 8;

void PrintErrorMsg(const char* const errMsg, const char* const filePath) {
#ifdef _DEBUG
  std::cout << "<PNG> " << (errMsg) << " : " << (filePath) << std::endl;
#endif
}

} // anonymous namespace

namespace cges::tex {

RenderBuffer cges::tex::LoadPng(const char* const filePath) {
  std::FILE* filePtr;
  {
    const auto err = fopen_s(&filePtr, filePath, "rb");
    if (!err) {
      PNG_EARLY_RETURN("file open failed", filePath);
    }
  }

  {
    png_byte header[8];
    std::fread(header, sizeof(png_byte), MAGICNUMBER_LENGTH, filePtr);
    if (png_sig_cmp(header, 0, MAGICNUMBER_LENGTH)) {
      PNG_EARLY_RETURN("incorrect magicnumber", filePath);
    }
  }

  auto pngInst = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!pngInst) {
    PNG_EARLY_RETURN("read_struct instantiation failed", filePath);
  }

  auto pngInfo = png_create_info_struct(pngInst);
  if (!pngInfo) {
    PNG_EARLY_RETURN("info_struct instantiation failed", filePath);
  }

  png_init_io(pngInst, filePtr);
  png_set_sig_bytes(pngInst, MAGICNUMBER_LENGTH);
  png_read_png(pngInst, pngInfo, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16, nullptr);

  const auto pngColorType = png_get_color_type(pngInst, pngInfo);
  if (pngColorType != PNG_COLOR_TYPE_RGB && pngColorType != PNG_COLOR_TYPE_RGBA) {
    png_destroy_read_struct(&pngInst, &pngInfo, nullptr);
    std::fclose(filePtr);
    PNG_EARLY_RETURN("color channel not capable", filePath);
  }

  png_bytepp rawImage = png_get_rows(pngInst, pngInfo);
  auto imageBuf = RenderBuffer(png_get_image_width(pngInst, pngInfo), png_get_image_height(pngInst, pngInfo));

  if (pngColorType == PNG_COLOR_TYPE_RGBA) {
    for (auto i = 0u; i < imageBuf.GetHeight(); ++i) {
      std::memcpy(imageBuf[i], rawImage[i], imageBuf.GetWidth() * 4);
    }
  }
  else {
    for (auto i = 0u; i < imageBuf.GetHeight(); ++i) {
      for (auto j = 0u; j < imageBuf.GetWidth(); ++j) {
        std::memcpy(&imageBuf[i][j], &rawImage[i][j], 3);
      }
    }
  }

  png_destroy_read_struct(&pngInst, &pngInfo, nullptr);
  std::fclose(filePtr);
  return GetErrorImage();
}

} // namespace cges::tex