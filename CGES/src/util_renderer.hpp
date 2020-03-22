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

private:
  RandomGenerator& m_rnd;
  size_t m_numTrials;
  const size_t m_recursionLimit;
  const float m_hitRate;    // 1�񎎍s���ăq�b�g����m��
  float m_continuationRate; // �p���� : �ċA�񐔂�LIMIT�ȏ�ɂȂ�ƒႭ�Ȃ��Ă���
};

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

bool IsInterpolatable(const RTCGeometryType geomType) noexcept;

bool WasIntersected(const unsigned int geomID) noexcept;

glm::vec3 SampleHemisphericalVec(const glm::vec3& org, const glm::vec3 normal) noexcept;

} // namespace cges