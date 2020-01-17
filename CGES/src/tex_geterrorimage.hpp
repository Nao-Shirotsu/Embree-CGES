#pragma once

#include "renderbuffer.hpp"

namespace cges::tex {

inline RenderBuffer GetErrorImage() {
  auto imageBuf = RenderBuffer(1, 1);
  imageBuf[0][0].r = 255u;
  imageBuf[0][0].b = 255u;
  return std::move(imageBuf);
}

} // namespace cges::tex