#include "renderer_base.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_sphere.hpp"
#include "util_renderer.hpp"

#include <thread>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace {

constexpr float SCREEN_WIDTH = 0.25;

}

namespace cges::renderer {

Base::Base()
  : m_maxThreads{ std::thread::hardware_concurrency() } {}

Base::~Base() {}

void Base::Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const {
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
    threads[y] = std::thread(&Base::ParallelDraw,
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

} // namespace cges::renderer