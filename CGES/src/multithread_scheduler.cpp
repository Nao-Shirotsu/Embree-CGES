#include "multithread_scheduler.hpp"

// debug
#include <iostream>
#include <exception>

namespace cges::multithread {

Scheduler::Scheduler(const RenderBuffer& renderTarget, const Scene& scene, const renderer::Base& renderer) noexcept
    : m_renderTarget(renderTarget)
    , m_scene(scene)
    , m_renderer(renderer)
    , m_renderProgress(renderTarget)
    , m_threads(std::thread::hardware_concurrency() / 3 * 2) {} // 2/3‚ÍƒmƒŠ‚ÅŒˆ‚ß‚Ä‚¢‚é

Scheduler::~Scheduler() {
}

void Scheduler::Dispatch() {
    for (auto& th : m_threads) {
      th = std::thread(
          [&] {
            while (!m_renderProgress.Completed()) {
              // ‚±‚±‚Érenderer::compute‚Æ‚©‚ğ‘‚­

              /*unsigned int w, h;
              m_renderProgress.GetNextIdx(w, h);
              std::cout << "[" << h << "][" << w << "]" << std::endl;
              ++m_renderProgress;*/
            }
          });
    }

    for (auto& th : m_threads) {
      th.join();
    }
}

}// namespace cges::multithread