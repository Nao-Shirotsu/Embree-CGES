#pragma once

#include "camera.hpp"
#include "renderbuffer.hpp"
#include "scene.hpp"

#include <glm/vec3.hpp>

#include <embree3/rtcore.h>

namespace cges::renderer {

class Base {
public:
  Base();
  Base(const size_t traceLowerLimit,
       const size_t traceUpperLimit,
       const size_t samplingLimit);
  virtual ~Base();

  void Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const;

  virtual void Update(const Camera& camera) = 0;

protected:
  // [loopMin, loopMax) ÇÃîÕàÕÇÃçsÇï`âÊÇ∑ÇÈ
  virtual void ParallelDraw(const Camera& camera, 
                            RenderBuffer& renderTarget, 
                            const Scene& scene,
                            const size_t loopMin,
                            const size_t loopMax,
                            const glm::vec3& initialPos,
                            const glm::vec3& screenVerticalVec,
                            const glm::vec3& screenHorizontalVec) const = 0;

  const size_t m_maxThreads;
  const size_t m_traceLowerLimit;
  const size_t m_traceUpperLimit;
  const size_t m_samplingLimit;
  size_t m_numSampling;
};

} // namespace cges::renderer