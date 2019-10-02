#pragma once

#include "camera.hpp"
#include "render_buffer.hpp"
#include "game_object.hpp"

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
  // [loopMin, loopMax) �͈̔͂̍s��`�悷��
  void ParallelDraw(const int loopMin,
                    const int loopMax,
                    const glm::vec3& initialPos,
                    const glm::vec3& screenVerticalVec,
                    const glm::vec3& screenHorizontalVec,
                    RTCIntersectContext* context) const;

  RTCDevice m_rtcDevice;
  RTCScene m_rtcScene;
  const Camera& m_camera;
  RenderBuffer& m_renderTarget;
  const uint32_t m_maxThreads;
};

} // namespace cges