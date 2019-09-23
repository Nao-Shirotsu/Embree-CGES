#pragma once

#include <cstdint>
#include <memory>
#include <fstream>
#include <iostream>

#include "color.hpp"

namespace shi {

// 描画ターゲットになるスクリーンバッファクラス
class RenderBuffer {
public:
  inline RenderBuffer(const size_t width, const size_t height)
      : m_width{ width }
      , m_height{ height }
      , m_pixels{ std::make_unique<ColorARGB[]>(width * height) } {
  }

  inline RenderBuffer(RenderBuffer&& other) noexcept
      : m_width{ other.m_width }
      , m_height{ other.m_height }
      , m_pixels{std::move(other.m_pixels)} {
  }

  inline RenderBuffer& operator=(RenderBuffer&& other) noexcept {
    if(&other == this) {
      return *this;
    }
    m_width = other.m_width;
    m_height = other.m_height;
    m_pixels = std::move(other.m_pixels);
    return *this;
  }

  inline ColorARGB* operator[](const size_t rowIdx) const noexcept(false){
    return m_pixels.get() + m_width * rowIdx;
  }

  inline size_t GetWidth() const noexcept{
    return m_width;
  }

  inline size_t GetHeight() const noexcept{
    return m_width;
  }

  // 成功でtrue, 失敗でfalseを返す
  inline bool SaveAsPpm(const char * const fileName) const noexcept{
    std::ofstream ofs(fileName);

    if (!ofs) {
      std::exit(1);
    }

    ofs << "P3\n"
        << m_width << " " << m_height << "\n255\n";
    for (int i = 0; i < m_height; ++i) {
      for (int j = 0; j < m_width; ++j) {
        const auto idx = i * m_width + j;
        ofs << static_cast<int>(m_pixels[idx].r) << " "
            << static_cast<int>(m_pixels[idx].g) << " "
            << static_cast<int>(m_pixels[idx].b) << "\n";
      }
    }

    ofs.close();
  }

private:
  RenderBuffer(const RenderBuffer&) = delete;
  RenderBuffer& operator=(const RenderBuffer&) = delete;

  size_t m_width;
  size_t m_height;
  std::unique_ptr<ColorARGB[]> m_pixels;
};

}// namespace shi