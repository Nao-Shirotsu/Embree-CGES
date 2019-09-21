#pragma once

#include "camera.hpp"
#include "render_buffer.hpp"
#include "object.hpp"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include <embree3/rtcore.h>

namespace {
constexpr float SCREEN_WIDTH = 4.0; // ワールド空間でのスクリーンの横幅
}

namespace cges {

class Renderer {
public:
  inline Renderer(const Camera& camera, const glm::vec3& lookingPos, RenderBuffer& renderTarget)
      : m_camera{ camera }
      , m_renderTarget{ renderTarget }
      , m_lookingPos{lookingPos} {
  }

  inline void Update() {
    RTCDevice device = rtcNewDevice(nullptr);
    if (!device) {
      return;
    }

    // ================= Init Scene  =================
    RTCScene scene = rtcNewScene(device);
    {
      auto object = cges::Object(device, scene);
      object.LoadObjFile("tetra.obj");
      object.AttachTo(scene);
    }
    rtcCommitScene(scene);
    // ===============================================


    const int32_t width = static_cast<int32_t>(m_renderTarget.GetWidth());
    const int32_t height = static_cast<int32_t>(m_renderTarget.GetHeight());
    const float aspectRatio = height / static_cast<float>(width);
    const glm::vec3 cameraFront = glm::normalize(m_lookingPos - m_camera.pos);
    const glm::vec3 screenHorizontalVec = glm::normalize(glm::cross(cameraFront, m_camera.upward)) * SCREEN_WIDTH;
    const glm::vec3 screenVerticalVec = glm::normalize(glm::cross(screenHorizontalVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
    const glm::vec3 screenCenterPos = m_camera.pos + cameraFront;

    // スクリーン左上(pixel[0][0])のワールド座標
    const glm::vec3 initialPos = screenCenterPos - screenVerticalVec - screenHorizontalVec;
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

    for (int y = 0; y < height; ++y) {
      const float yRate = y / static_cast<float>(height);
      for (int x = 0; x < width; ++x) {
        const float xRate = x / static_cast<float>(width);
        const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
        const glm::vec3 rayDir = pixelPos - m_camera.pos;
        RTCRayHit rayhit;
        rayhit.ray.org_x = m_camera.pos.x;
        rayhit.ray.org_y = m_camera.pos.y;
        rayhit.ray.org_z = m_camera.pos.z;
        rayhit.ray.dir_x = rayDir.x;
        rayhit.ray.dir_y = rayDir.y;
        rayhit.ray.dir_z = rayDir.z;
        rayhit.ray.tnear = 0;
        rayhit.ray.tfar = 100000000; // 適当
        rayhit.ray.mask = 0;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
        rtcIntersect1(scene, &context, &rayhit);
        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
          m_renderTarget[y][x].r = 255;
          m_renderTarget[y][x].b = 255;
        }
      }
    }
    rtcReleaseScene(scene);
    rtcReleaseDevice(device);
  }

  inline void Draw() const{
    m_renderTarget.SaveAsPpm("out.ppm");
  }

private:
  const Camera& m_camera;
  const RenderBuffer& m_renderTarget;
  glm::vec3 m_lookingPos;
};

} // namespace cges