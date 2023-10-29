#pragma once

#include "color.hpp"

#include <algorithm>
#include <limits> // float minimum

#include <glm/vec3.hpp>

namespace cges {

namespace constants {

// math
constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();
constexpr float PI = 3.14159265358979323846f;

// Color
constexpr ColorRGBA BG_COLOR = { 64, 128, 128, 255 };

// config defaults
constexpr float INITIAL_ROULETTE_HIT_RATE = 0.25f;

}// namespace constants

inline constexpr float Saturate(const float x) noexcept {
  return std::clamp(x, 0.0f, 1.0f);
}

inline constexpr double Saturate(const double x) noexcept {
  return std::clamp(x, 0.0, 1.0);
}

inline constexpr uint8_t Saturate(const uint8_t x) noexcept {
  return std::clamp(x, static_cast<uint8_t>(0x00), static_cast<uint8_t>(0xff));
}

// convert from [0.0, 1.0) to [0, 255]
inline constexpr uint8_t ToUInt8(const float x) noexcept {
  return static_cast<uint8_t>(x * 255.0f);
}

// convert glm::vec3 as color to cges::ColorRGBA
// 変換コンストラクタの方がよくね？ まあいいか...
inline constexpr ColorRGBA ToColorUInt(const glm::vec3& colorf) noexcept {
  return {
    ToUInt8(colorf.r),
    ToUInt8(colorf.g),
    ToUInt8(colorf.b),
    0xff
  };
}

inline float GammaCorrect(const float intensity) noexcept {
  return std::powf(intensity, 1.0f / 2.2f);
}

}// namsespace cges