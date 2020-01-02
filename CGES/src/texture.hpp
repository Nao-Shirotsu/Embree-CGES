#pragma once

#include "renderbuffer.hpp"

#include <glm/vec3.hpp>

namespace cges {

class Texture {
public:
  Texture(const char* const filePath);

  // テクスチャ座標を浮動小数で指定して色を取得
  ColorRGBA GetPixel(const float x, const float y) const noexcept;

#ifdef _DEBUG
  void SaveAsPpm(const char* const filePath);
#endif

private:
  RenderBuffer m_imageBuffer;
};

}// namespace cges