#include "render_buffer.hpp"

#include <fstream>

namespace cges {

cges::RenderBuffer::RenderBuffer(const size_t width, const size_t height)
    : m_width{ width }
    , m_height{ height }
    , m_pixels{ std::make_unique<ColorRGBA[]>(width * height) } {}

RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    : m_width{ other.m_width }
    , m_height{ other.m_height }
    , m_pixels{ std::move(other.m_pixels) } {
}

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept {
  if (&other == this) {
    return *this;
  }
  m_width = other.m_width;
  m_height = other.m_height;
  m_pixels = std::move(other.m_pixels);
  return *this;
}

ColorRGBA* RenderBuffer::operator[](const size_t rowIdx) const noexcept(false) {
  return m_pixels.get() + m_width * rowIdx;
}

size_t RenderBuffer::GetWidth() const noexcept {
  return m_width;
}

size_t RenderBuffer::GetHeight() const noexcept {
  return m_height;
}

bool RenderBuffer::SaveAsPpm(const char* const fileName) const noexcept {
  std::ofstream ofs(fileName);

  if (!ofs) {
    std::exit(1);
  }

  ofs << "P3\n"
      << m_width << " " << m_height << "\n255\n";
  for (size_t i = 0; i < m_height; ++i) {
    for (size_t j = 0; j < m_width; ++j) {
      const auto idx = i * m_width + j;
      ofs << static_cast<int>(m_pixels[idx].r) << " "
          << static_cast<int>(m_pixels[idx].g) << " "
          << static_cast<int>(m_pixels[idx].b) << "\n";
    }
  }

  ofs.close();
  return true;
}

}