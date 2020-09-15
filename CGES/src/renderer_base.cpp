#include "renderer_base.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_sphere.hpp"
#include "util_renderer.hpp"

#include <thread>
#include <vector>
#include <memory>
#include <iostream>

#include <glm/glm.hpp>

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float SCREEN_WIDTH = 0.25f;

float DegToRad(const float degree) {
  return degree * PI / 180.0f;
}

}

namespace cges::renderer {
Base::Base(Method method)
    : m_maxThreads(std::thread::hardware_concurrency())
    , m_traceLowerLimit(0)
    , m_traceUpperLimit(0)
    , m_samplingLimit(2)
    , m_numSampling(0)
    , m_method(method){
}

Base::Base(const size_t traceLowerLimit, const size_t traceUpperLimit, const size_t samplingLimit, Method method)
    : m_maxThreads(std::thread::hardware_concurrency())
    , m_traceLowerLimit(traceLowerLimit)
    , m_traceUpperLimit(traceUpperLimit)
    , m_samplingLimit(samplingLimit)
    , m_numSampling(1) 
    , m_method(method){
}

Base::~Base() {}

void Base::Draw(const Camera& camera, RenderBuffer& renderTarget, const Scene& scene) const {
  if (m_numSampling > m_samplingLimit) {
    return;
  }

  const auto width = renderTarget.GetWidth();
  const auto height = renderTarget.GetHeight();
  const float aspectRatio = height / static_cast<float>(width);
  const glm::vec3 cameraFront = glm::normalize(glm::vec3{0, 0, 0} - camera.GetPosLocal());
  const glm::vec3 screenHorizontalVec = glm::normalize(glm::cross(camera.upwardWorld, cameraFront)) * SCREEN_WIDTH;
  const glm::vec3 screenVerticalVec = glm::normalize(glm::cross(screenHorizontalVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
  const auto distanceToScreen = std::tanf(DegToRad((180.0f - camera.fov) / 2.0f)) * SCREEN_WIDTH / 2.0f;
  const glm::vec3 screenCenterPos = camera.posWorld + camera.GetPosLocal() + cameraFront * distanceToScreen;

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

Method Base::RenderMethod() {
  return m_method;
}

} // namespace cges::renderer