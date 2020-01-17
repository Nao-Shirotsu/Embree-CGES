#include "tex_loadpng.hpp"
#include "tex_geterrorimage.hpp"

namespace cges::tex {

RenderBuffer cges::tex::LoadPng(const char* const filePath) {
  return GetErrorImage();
}

} // namespace cges::tex