#include "renderer.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_sphere.hpp"
#include "util_renderer.hpp"

#include <thread>
#include <vector>
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

Renderer::Renderer()
  : m_maxThreads{ std::thread::hardware_concurrency() } {}

Renderer::~Renderer() {}

void cges::Renderer::Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const {
  const int32_t width = static_cast<int32_t>(renderTarget.GetWidth());
  const int32_t height = static_cast<int32_t>(renderTarget.GetHeight());
  const float aspectRatio = height / static_cast<float>(width);
  const glm::vec3 cameraFront = glm::normalize(glm::vec3{0, 0, 0} - camera.GetPosLocal());
  const glm::vec3 screenHorizontalVec = glm::normalize(glm::cross(camera.upwardWorld, cameraFront)) * SCREEN_WIDTH;
  const glm::vec3 screenVerticalVec = glm::normalize(glm::cross(screenHorizontalVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
  const glm::vec3 screenCenterPos = camera.posWorld + camera.GetPosLocal() + cameraFront;

  // スクリーン左上(pixel[0][0])のワールド座標
  const glm::vec3 initialPos = screenCenterPos - (glm::vec3{ 0.5, 0.5, 0.5 } * screenVerticalVec) - (glm::vec3{ 0.5, 0.5, 0.5 } * screenHorizontalVec);

  if (!m_maxThreads) {
    ParallelDraw(std::cref(camera),
                 std::ref(renderTarget),
                 std::cref(scene),
                 0,
                 height,
                 initialPos,
                 screenVerticalVec,
                 screenHorizontalVec);
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
                             std::cref(camera),
                             std::ref(renderTarget),
                             std::cref(scene),
                             loopBegIdx,
                             loopEndIdx,
                             initialPos,
                             screenVerticalVec,
                             screenHorizontalVec);
  }
  for (auto& th : threads) {
    th.join();
  }
}

void Renderer::ParallelDraw(const Camera& camera,
                            RenderBuffer& renderTarget,
                            const Scene& scene, 
                            const int loopMin, 
                            const int loopMax, 
                            const glm::vec3& initialPos, 
                            const glm::vec3& screenVerticalVec, 
                            const glm::vec3& screenHorizontalVec) const {
  const auto cameraPos = camera.posWorld + camera.GetPosLocal();
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1;
    for (size_t x = 0; x < renderTarget.GetWidth(); ++x) {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      const float xRate = x / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = pixelPos - cameraPos;
      RTCRayHit rayhit;
      InitRayHit(rayhit, cameraPos, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][x].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][x].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][x].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }
      renderTarget[yIdx][x] = scene.GetGeomColor(rayhit.hit.geomID, 0.0f, 0.0f);

      // shading
      glm::vec3 faceNormal = {0, 0, 0};
      if (IsInterpolatable(scene.GetGeomType(rayhit.hit.geomID))) {
        rtcInterpolate0(rtcGetGeometry(scene.GetRTCScene(), rayhit.hit.geomID),
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
      const glm::vec3 reflectedDir = scene.GetDirLightForward() - 2.0f * glm::dot(scene.GetDirLightForward(), faceNormal) * faceNormal;
      const float specularFactor = std::clamp(glm::dot(glm::normalize(reflectedDir), glm::normalize(cameraPos)), 0.0f, 1.0f);
      const float diffuseFactor = std::clamp(glm::dot(-scene.GetDirLightForward(), faceNormal), 0.0f, 1.0f);
      const ColorRGBA objColor = scene.GetGeomColor(rayhit.hit.geomID, rayhit.hit.u, rayhit.hit.v);
      renderTarget[yIdx][x].r = std::clamp(static_cast<int>(objColor.r * diffuseFactor + 96 * specularFactor) + 32, 0, 255);
      renderTarget[yIdx][x].g = std::clamp(static_cast<int>(objColor.g * diffuseFactor + 96 * specularFactor) + 16, 0, 255);
      renderTarget[yIdx][x].b = std::clamp(static_cast<int>(objColor.b * diffuseFactor + 96 * specularFactor) + 16, 0, 255);
    }
  }
}

} // namespace cges::obj