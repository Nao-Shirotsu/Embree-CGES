#pragma once

#include "color.hpp"

#include <cstdint>
#include <memory>

namespace cges {

// 描画ターゲットになるスクリーンバッファクラス
class RenderBuffer {
public:
  RenderBuffer(const size_t width, const size_t height);
  RenderBuffer(RenderBuffer&& other) noexcept;

  RenderBuffer& operator=(RenderBuffer&& other) noexcept;
  ColorRGBA* operator[](const size_t rowIdx) const noexcept(false);

  size_t GetWidth() const noexcept;
  size_t GetHeight() const noexcept;

  // 成功でtrue, 失敗でfalseを返す
  bool SaveAsPpm(const char* const fileName) const noexcept;

private:
  RenderBuffer(const RenderBuffer&) = delete;
  RenderBuffer& operator=(const RenderBuffer&) = delete;

  size_t m_width;
  size_t m_height;
  std::unique_ptr<ColorRGBA[]> m_pixels;
};

} // namespace cges