#include "multithread_scheduler.hpp"

#include <iostream>

namespace {

void println(const char* str) {
  std::cout << str << std::endl;
}

}

namespace cges::multithread {

Scheduler::Scheduler(const Camera& camera, const RenderBuffer& renderTarget) noexcept
    : m_screenGeom(camera, renderTarget)
    , m_renderProgress(renderTarget)
    , m_threads(std::thread::hardware_concurrency() / 3 * 2) {} // 2/3はノリで決めている

Scheduler::~Scheduler() {
}

void Scheduler::Dispatch(RenderBuffer& renderTarget, const Scene& scene, const renderer::Base& renderer) {
  //for (auto& th : m_threads) {
  //  th = std::thread(
  //      [&] {
  //        unsigned int yIdx, xIdx;
  //        while (!m_renderProgress.Completed()) {
  //          m_renderProgress.GetNextIdx(yIdx, xIdx);
  //          ++m_renderProgress;
  //          const auto pixelPos = m_screenGeom.ComputePixelPos(yIdx, xIdx);
  //          const auto dir = m_screenGeom.ComputeDirection(yIdx, xIdx);
  //          ref_renderTarget[yIdx][xIdx] = renderer.ComputeLightTransport(scene, pixelPos, dir);
  //        }
  //      });
  //}

  /*for (auto& th : m_threads) {
    th.join();
  }*/

  // ===================↑正しい画像が出力されないのでスレッド機能オフでデバッグ======================
  unsigned int yIdx, xIdx;
  while (!m_renderProgress.Completed()) {
    m_renderProgress.GetNextIdx(yIdx, xIdx);
    ++m_renderProgress;
    const auto pixelPos = m_screenGeom.ComputePixelPos(yIdx, xIdx);
    const auto dir = m_screenGeom.ComputeDirection(yIdx, xIdx);
    // renderTarget[yIdx][xIdx] = renderer.ComputeLightTransport(scene, pixelPos, dir);
    auto color = renderer.ComputeLightTransport(scene, pixelPos, dir);
    renderTarget[yIdx][xIdx] = color;
  }
}

} // namespace cges::multithread