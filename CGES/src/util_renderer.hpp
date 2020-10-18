#pragma once

#include "scene.hpp"
#include "randomgenerator.hpp"

#include <glm/vec3.hpp>
#include <embree3/rtcore_ray.h>
#include <embree3/rtcore_geometry.h>

namespace cges {

class RussianRoulette {
public:
  RussianRoulette(RandomGenerator& rnd, const size_t lowerLimit, const size_t upperLimit, const float hitRate);
  bool Spin() noexcept;
  void SetContinueRate(const float rate) noexcept;
  float ContinueRate() const noexcept;

private:
  RandomGenerator& m_rnd;
  size_t m_numTrials;
  const size_t m_lowerRecursionLimit;
  const size_t m_upperRecursionLimit;
  float m_continuationRate; // Œp‘±—¦ : Ä‹A‰ñ”‚ªLIMITˆÈã‚É‚È‚é‚Æ’á‚­‚È‚Á‚Ä‚¢‚­
};

struct WrappedRayHit{
public:
  WrappedRayHit(const glm::vec3& org, const glm::vec3& dir);

  bool Intersect(const Scene& scene, RTCIntersectContext& context);

  // Getters(’P‚É‰Â“Ç«‚Ì‚½‚ß)
  glm::vec3 RayOrigin() const noexcept;
  glm::vec3 RayDir() const noexcept;
  glm::vec3 HitNormal() const noexcept; // –¢³‹K‰»
  float HitU() const noexcept;
  float HitV() const noexcept;
  float RayTnear() const noexcept;
  float RayTfar() const noexcept;
  float RayTime() const noexcept;
  unsigned int RayMask() const noexcept;
  unsigned int RayId() const noexcept;
  unsigned int RayFlags() const noexcept;
  unsigned int HitPrimId() const noexcept;
  unsigned int HitGeomId() const noexcept;

  // vec3Œ^‚Å‚Å•Û‚Å‚«‚È‚¢‚½‚ßprivate‚É‚Í‚µ‚È‚¢
  RTCRayHit rayhit;
};

void InitRayHit(RTCRayHit& rayhit, const glm::vec3& org, const glm::vec3& dir) noexcept;

bool IsInterpolatable(const RTCGeometryType geomType) noexcept;

bool WasIntersected(const unsigned int geomID) noexcept;

} // namespace cges