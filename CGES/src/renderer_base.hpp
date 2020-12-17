#pragma once

#include "camera.hpp"
#include "renderbuffer.hpp"
#include "scene.hpp"

#include <glm/vec3.hpp>

#include <embree3/rtcore.h>

namespace cges::renderer {

enum class Method {
  PhongShading,
  Pathtracing,
  NEEPathtracing,
  PhotonMapping,
};

class Base {
public:
  Base(const Method method);
  Base(const size_t traceLowerLimit,
       const size_t traceUpperLimit,
       const size_t samplingLimit,
       const Method method);
  virtual ~Base();

  virtual void Update(const Camera& camera) = 0;

  void Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const;

  Method RenderMethod();

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
  const Method m_method;
};

} // namespace cges::renderer