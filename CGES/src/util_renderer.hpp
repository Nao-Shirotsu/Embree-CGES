#pragma once

#include <random>
#include <glm/vec3.hpp>
#include <embree3/rtcore_ray.h>
#include <embree3/rtcore_geometry.h>

namespace cges {

class RandomGenerator {
public:
  RandomGenerator();
  float operator()();

private:
  std::mt19937_64 mt;
  std::uniform_real_distribution<float> distr;
};

class RussianRoulette {
public:
  RussianRoulette(RandomGenerator& rnd, const size_t limit, const float hitRate);
  bool Spin() noexcept;
  void SetContinueRate(const float rate);

private:
  RandomGenerator& m_rnd;
  size_t m_numTrials;
  const size_t m_recursionLimit;
  float m_continuationRate; // Œp‘±—¦ : Ä‹A‰ñ”‚ªLIMITˆÈã‚É‚È‚é‚Æ’á‚­‚È‚Á‚Ä‚¢‚­
};

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

bool IsInterpolatable(const RTCGeometryType geomType) noexcept;

bool WasIntersected(const unsigned int geomID) noexcept;

} // namespace cges