#pragma once

#include "camera.hpp"
#include "render_buffer.hpp"
#include "game_object.hpp"

#include <glm/vec3.hpp>

#include <embree3/rtcore.h>

namespace cges {

class Renderer {
public:
  Renderer(const Camera& camera, const glm::vec3& lookingPos, RenderBuffer& renderTarget);
  ~Renderer();

  void Update();
  void Draw() const;

private:
  RTCDevice m_rtcDevice;
  RTCScene m_rtcScene;
  const Camera& m_camera;
  RenderBuffer& m_renderTarget;
  glm::vec3 m_lookingPos;
  const uint32_t m_maxThreads;
};

} // namespace cges