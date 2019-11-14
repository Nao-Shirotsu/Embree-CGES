#pragma once

#include "renderbuffer.hpp"



namespace cges {

class Texture : public RenderBuffer{
public:
  Texture(const size_t width, const size_t height);

  // 成功でtrue, 失敗でfalseを返す
  bool LoadPpm(const char* const filePath);

  // テクスチャ座標を浮動小数で指定して色を取得
  ColorRGBA GetPixel(const float x, const float y);
};

}// namespace cges