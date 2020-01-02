#include "renderer.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_sphere.hpp"

#include <thread>
#include <vector>
#include <limits> // float infinity
#include <cmath>
#include <algorithm> // clamp
#include <memory>

#include <glm/glm.hpp>

namespace {

constexpr float SCREEN_WIDTH = 0.25;
bool IsInterpolatable(const RTCGeometryType geomType) {
  if(geomType == RTC_GEOMETRY_TYPE_GRID ||
     geomType == RTC_GEOMETRY_TYPE_INSTANCE ||
     geomType == RTC_GEOMETRY_TYPE_DISC_POINT ||
     geomType == RTC_GEOMETRY_TYPE_SPHERE_POINT || 
     geomType == RTC_GEOMETRY_TYPE_USER) {
    return false;
  }
  return true;
}

bool WasIntersected(const unsigned int geomID) {
  return geomID != static_cast<int>(-1);
}

}

namespace cges {

Renderer::Renderer(const Camera& camera, RenderBuffer& renderTarget)
    : m_rtcDevice{ rtcNewDevice(nullptr) }
    , m_scene{ m_rtcDevice }
    , m_camera{ camera }
    , m_renderTarget{ renderTarget }
    , m_maxThreads{ std::thread::hardware_concurrency() } {
  //m_scene.Add(MakePolygonalMesh(m_rtcDevice, "bin/goat_filled.obj", {64, 64, 255}));
  m_scene.Add(MakeSphere(m_rtcDevice, 1.25f, "CBfront.ppm"));
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

      if (!WasIntersected(rayhit.hit.geomID)) {
        m_renderTarget[yIdx][x].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        m_renderTarget[yIdx][x].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        m_renderTarget[yIdx][x].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }
      m_renderTarget[yIdx][x] = m_scene.GetGeomColor(rayhit.hit.geomID, 0.0f, 0.0f);

      // shading
      glm::vec3 faceNormal = {0, 0, 0};
      if (IsInterpolatable(m_scene.GetGeomType(rayhit.hit.geomID))) {
        rtcInterpolate0(rtcGetGeometry(m_scene.GetRTCScene(), rayhit.hit.geomID),
                        rayhit.hit.primID,
                        rayhit.hit.u,
                        rayhit.hit.v,
                        RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                        0,
                        reinterpret_cast<float*>(&faceNormal),
                        3);
      }
      else {
        faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
      }
      const glm::vec3 reflectedDir = m_scene.GetDirLightForward() - 2.0f * glm::dot(m_scene.GetDirLightForward(), faceNormal) * faceNormal;
      const float specularFactor = std::clamp(glm::dot(glm::normalize(reflectedDir), glm::normalize(cameraPos)), 0.0f, 1.0f);
      const float diffuseFactor = std::clamp(glm::dot(-m_scene.GetDirLightForward(), faceNormal), 0.0f, 1.0f);
      const ColorRGBA objColor = m_scene.GetGeomColor(rayhit.hit.geomID, rayhit.hit.u, rayhit.hit.v);
      m_renderTarget[yIdx][x].r = std::clamp(static_cast<int>(objColor.r * diffuseFactor + 96 * specularFactor) + 32, 0, 255);
      m_renderTarget[yIdx][x].g = std::clamp(static_cast<int>(objColor.g * diffuseFactor + 96 * specularFactor) + 16, 0, 255);
      m_renderTarget[yIdx][x].b = std::clamp(static_cast<int>(objColor.b * diffuseFactor + 96 * specularFactor) + 16, 0, 255);
    }
  }
}

} // namespace cges::obj