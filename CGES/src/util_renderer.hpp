#pragma once

#include <random>
#include <glm/vec3.hpp>
#include <embree3/rtcore_ray.h>
#include <embree3/rtcore_geometry.h>

class RandomGen {
public:
  RandomGen()
      : mt(std::random_device()())
      , rand(0.0f, 1.0f) {}

  float operator()() {
    return rand(mt);
  }

private:
  std::mt19937_64 mt;
  std::uniform_real_distribution<float> rand;
};

namespace cges {

class RussianRoulette {
public:
  RussianRoulette(const size_t limit, const float hitRate);
  bool Spin() noexcept;

private:
  size_t m_numTrials;
  const size_t m_recursionLimit;
  const float m_hitRate;    // 1回試行してヒットする確率
  float m_continuationRate; // 継続率 : 再帰回数がLIMIT以上になると低くなっていく
};

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

bool IsInterpolatable(const RTCGeometryType geomType) noexcept;

bool WasIntersected(const unsigned int geomID) noexcept;

} // namespace cges