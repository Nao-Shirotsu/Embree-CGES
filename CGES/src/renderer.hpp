#pragma once

#include "camera.hpp"
#include "render_buffer.hpp"
#include "game_object.hpp"
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
                    const glm::vec3& screenHorizontalVec,
                    RTCIntersectContext* context) const;
  
  ColorRGBA GetNormalPixel(const glm::vec3& faceNormal) const;
  ColorRGBA GetUVPixel(const RTCHit& rayhit) const;
  ColorRGBA GetPhongDiffusePixel(const glm::vec3& faceNormal) const;
  ColorRGBA GetPhongSpecularPixel(const glm::vec3& faceNormal, const glm::vec3& cameraPos) const;
  constexpr ColorRGBA GetPhongAmbientPixel() const{
    return { 32, 16, 16, 0 };
  }
  ColorRGBA GetPhongShadingPixel(const glm::vec3& faceNormal, const glm::vec3& cameraPos) const;
  ColorRGBA GetBGPixel(const float colorIntensity) const;

  RTCDevice m_rtcDevice;
  Scene m_scene;
  const Camera& m_camera;
  RenderBuffer& m_renderTarget;
  const uint32_t m_maxThreads;
};

} // namespace cges