#pragma once

#include "camera.hpp"
#include "renderbuffer.hpp"
#include "scene.hpp"

#include <glm/vec3.hpp>

#include <embree3/rtcore.h>

namespace cges {

class Renderer {
public:
  Renderer();
  ~Renderer();

  void Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const;

protected:
  // [loopMin, loopMax) �͈̔͂̍s��`�悷��
  void ParallelDraw(const Camera& camera, 
                    RenderBuffer& renderTarget, 
                    const Scene& scene,
                    const int loopMin,
                    const int loopMax,
                    const glm::vec3& initialPos,
                    const glm::vec3& screenVerticalVec,
                    const glm::vec3& screenHorizontalVec) const;

  const uint32_t m_maxThreads;
};

} // namespace cges