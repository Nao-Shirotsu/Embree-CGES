#pragma once

#include "camera.hpp"
#include "renderbuffer.hpp"
#include "scene.hpp"

#include <glm/vec3.hpp>

#include <embree3/rtcore.h>

namespace cges {

class Renderer {
public:
  Renderer(const Camera& camera, RenderBuffer& renderTarget);
  ~Renderer();

  void Update();
  void Draw() const;

private:
  // [loopMin, loopMax) ÇÃîÕàÕÇÃçsÇï`âÊÇ∑ÇÈ
  void ParallelDraw(const int loopMin,
                    const int loopMax,
                    const glm::vec3& initialPos,
                    const glm::vec3& screenVerticalVec,
                    const glm::vec3& screenHorizontalVec) const;

  RTCDevice m_rtcDevice;
  Scene m_scene;
  const Camera& m_camera;
  RenderBuffer& m_renderTarget;
  const uint32_t m_maxThreads;
};

} // namespace cges