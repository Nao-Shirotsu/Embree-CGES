#include "renderer.hpp"

#include <thread>
#include <vector>
#include <numeric> // float infinity
#include <cmath>
#include <algorithm> // clamp

#include <glm/glm.hpp>

namespace {

constexpr float SCREEN_WIDTH = 0.25;

}

namespace cges {

Renderer::Renderer(const Camera& camera, RenderBuffer& renderTarget)
    : m_rtcDevice{ rtcNewDevice(nullptr) }
    , m_scene{ m_rtcDevice }
    , m_camera{ camera }
    , m_renderTarget{ renderTarget }
    , m_maxThreads{ std::thread::hardware_concurrency() } {
  m_scene.Emplace(m_rtcDevice, "bin/cat.obj");
}

Renderer::~Renderer() {
  rtcReleaseDevice(m_rtcDevice);
}

void Renderer::Update() {
  m_scene.Update();
}

void cges::Renderer::Draw() const {
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  const int32_t width = static_cast<int32_t>(m_renderTarget.GetWidth());
  const int32_t height = static_cast<int32_t>(m_renderTarget.GetHeight());
  const float aspectRatio = height / static_cast<float>(width);
  const glm::vec3 cameraFront = glm::normalize(glm::vec3{0, 0, 0} - m_camera.GetPosLocal());
  const glm::vec3 screenHorizontalVec = glm::normalize(glm::cross(m_camera.upwardWorld, cameraFront)) * SCREEN_WIDTH;
  const glm::vec3 screenVerticalVec = glm::normalize(glm::cross(screenHorizontalVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
  const glm::vec3 screenCenterPos = m_camera.posWorld + m_camera.GetPosLocal() + cameraFront;

  // スクリーン左上(pixel[0][0])のワールド座標
  const glm::vec3 initialPos = screenCenterPos - (glm::vec3{ 0.5, 0.5, 0.5 } * screenVerticalVec) - (glm::vec3{ 0.5, 0.5, 0.5 } * screenHorizontalVec);

  if (!m_maxThreads) {
    ParallelDraw(0,
                 height,
                 initialPos,
                 screenVerticalVec,
                 screenHorizontalVec,
                 &context);
  }

  auto threads = std::vector<std::thread>(m_maxThreads);
  const auto numLoop = height / m_maxThreads;

  for (size_t y = 0; y < m_maxThreads; ++y) {
    size_t loopBegIdx = numLoop * y;
    size_t loopEndIdx = loopBegIdx + numLoop;
    if (y == m_maxThreads - 1) {
      loopEndIdx += (height % m_maxThreads);
    }
    threads[y] = std::thread(&Renderer::ParallelDraw,
                             this,
                             loopBegIdx,
                             loopEndIdx,
                             initialPos,
                             screenVerticalVec,
                             screenHorizontalVec,
                             &context);
  }
  for (auto& th : threads) {
    th.join();
  }
}

void Renderer::ParallelDraw(const int loopMin, const int loopMax, const glm::vec3& initialPos, const glm::vec3& screenVerticalVec, const glm::vec3& screenHorizontalVec, RTCIntersectContext* context) const{
  const auto cameraPos = m_camera.posWorld + m_camera.GetPosLocal();
  for (size_t y = loopMin; y < loopMax; ++y) {
    const size_t height = m_renderTarget.GetHeight();
    const float yRate = y / static_cast<float>(height);
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t width = m_renderTarget.GetWidth();
    const size_t yIdx = height - y - 1;
    for (size_t x = 0; x < width; ++x) {
      const float xRate = x / static_cast<float>(width);
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = pixelPos - cameraPos;
      RTCRayHit rayhit;
      rayhit.ray.org_x = cameraPos.x;
      rayhit.ray.org_y = cameraPos.y;
      rayhit.ray.org_z = cameraPos.z;
      rayhit.ray.dir_x = rayDir.x;
      rayhit.ray.dir_y = rayDir.y;
      rayhit.ray.dir_z = rayDir.z;
      rayhit.ray.tnear = 0;
      rayhit.ray.tfar = std::numeric_limits<float>::max();
      rayhit.ray.mask = 0;
      rayhit.ray.flags = 0;
      rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rtcIntersect1(m_scene.GetRTCScene(), context, &rayhit);

      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        //const auto geomNormal = glm::normalize(glm::vec3{ rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z });
        glm::vec3 faceNormal = { 0, 0, 0 };
        rtcInterpolate0(rtcGetGeometry(m_scene.GetRTCScene(), rayhit.hit.geomID),
                        rayhit.hit.primID,
                        rayhit.hit.u,
                        rayhit.hit.v,
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                        0,
                        reinterpret_cast<float*>(&faceNormal),
                        3);
        faceNormal = glm::normalize(faceNormal);
        switch(m_camera.mode) {
          case RenderMode::NORMAL: {
            m_renderTarget[yIdx][x] = GetNormalPixel(faceNormal);
          } break;

          case RenderMode::UV: {
            m_renderTarget[yIdx][x] = GetUVPixel(rayhit.hit);
          } break;

          case RenderMode::PHONG_DIFFUSE: {
            m_renderTarget[yIdx][x] = GetPhongDiffusePixel(faceNormal);
          } break;

          case RenderMode::PHONG_SPECULAR: {
            m_renderTarget[yIdx][x] = GetPhongSpecularPixel(faceNormal, cameraPos);
          } break;
            
          case RenderMode::PHONG_AMBIENT: {
            m_renderTarget[yIdx][x] = GetPhongAmbientPixel();
          } break;

          case RenderMode::PHONG_SHADING: {
            m_renderTarget[yIdx][x] = GetPhongShadingPixel(faceNormal, cameraPos);
          } break;

          default: {
            m_renderTarget[yIdx][x] = GetBGPixel(bgColorIntensity);
          }
        }
      }
      else {
        m_renderTarget[yIdx][x] = GetBGPixel(bgColorIntensity);
      }
    }
  }
}

ColorRGBA Renderer::GetNormalPixel(const glm::vec3& faceNormal) const{
  return ColorRGBA {
    static_cast<uint8_t>(255 * std::clamp(faceNormal.x, 0.0f, 1.0f)),
    static_cast<uint8_t>(255 * std::clamp(faceNormal.y, 0.0f, 1.0f)),
    static_cast<uint8_t>(255 * std::clamp(faceNormal.z, 0.0f, 1.0f)),
    0
  };
}

ColorRGBA Renderer::GetUVPixel(const RTCHit& rayhit) const{
  return ColorRGBA{
    static_cast<uint8_t>(0),
    static_cast<uint8_t>(255 * rayhit.u),
    static_cast<uint8_t>(255 * rayhit.v),
    0
  };
}

ColorRGBA Renderer::GetPhongDiffusePixel(const glm::vec3& faceNormal) const{
  const glm::vec3 reflectedDir = m_scene.GetDirLightForward() - 2.0f * glm::dot(m_scene.GetDirLightForward(), faceNormal) * faceNormal;
  const float diffuseFactor = std::clamp(glm::dot(-m_scene.GetDirLightForward(), faceNormal), 0.0f, 1.0f);
  return ColorRGBA{
    static_cast<uint8_t>(std::clamp(255 * diffuseFactor, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(64 * diffuseFactor, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(64 * diffuseFactor, 0.0f, 255.0f)),
    0
  };
}

ColorRGBA Renderer::GetPhongSpecularPixel(const glm::vec3& faceNormal, const glm::vec3& cameraPos) const {
  const glm::vec3 reflectedDir = m_scene.GetDirLightForward() - 2.0f * glm::dot(m_scene.GetDirLightForward(), faceNormal) * faceNormal;
  const float specularFactor = std::clamp(glm::dot(glm::normalize(reflectedDir), glm::normalize(cameraPos)), 0.0f, 1.0f);
  return ColorRGBA{
    static_cast<uint8_t>(std::clamp(128 * specularFactor, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(128 * specularFactor, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(128 * specularFactor, 0.0f, 255.0f)),
    0
  };
}

ColorRGBA Renderer::GetPhongShadingPixel(const glm::vec3& faceNormal, const glm::vec3& cameraPos) const {
  const glm::vec3 reflectedDir = m_scene.GetDirLightForward() - 2.0f * glm::dot(m_scene.GetDirLightForward(), faceNormal) * faceNormal;
  const float specularFactor = std::clamp(glm::dot(glm::normalize(reflectedDir), glm::normalize(cameraPos)), 0.0f, 1.0f);
  const float diffuseFactor = std::clamp(glm::dot(-m_scene.GetDirLightForward(), faceNormal), 0.0f, 1.0f);
  return ColorRGBA{
    static_cast<uint8_t>(std::clamp(255 * diffuseFactor + 128 * specularFactor + 32, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(64 * diffuseFactor + 128 * specularFactor + 16, 0.0f, 255.0f)),
    static_cast<uint8_t>(std::clamp(64 * diffuseFactor + 128 * specularFactor + 16, 0.0f, 255.0f)),
    0
  };
}

ColorRGBA Renderer::GetBGPixel(const float colorIntensity) const {
  return ColorRGBA{
    static_cast<uint8_t>(colorIntensity / 1.5f),
    static_cast<uint8_t>(colorIntensity / 1.5f),
    static_cast<uint8_t>(colorIntensity),
    0
  };
}

} // namespace cges::obj