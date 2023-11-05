#pragma once

#include "renderbuffer.hpp"
#include "scene.hpp"
#include "renderer_base.hpp"
#include "multithread_mutexrenderprogress.hpp"

#include <thread>
#include <vector>

namespace cges::multithread {

// 1. ctorとかでRenderer, Scene, あと何だ？を受け取る
// 2. exec みたいな関数で height width ぶんループまわす
// 3. ループ内でメンバ変数のスレッドたちに1pxぶんの処理を渡してやってもらう(適宜イテレータをmutex guardして進めていく)
// 4. 最後までいったら完了

class Scheduler {
public:
  Scheduler(const RenderBuffer& renderTarget, const Scene& scene, const renderer::Base& renderer) noexcept;
  ~Scheduler();
  void Dispatch();

private:
  const RenderBuffer& m_renderTarget;
  const Scene& m_scene;
  const renderer::Base& m_renderer;
  MutexRenderProgress m_renderProgress; 
  std::vector<std::thread> m_threads; // hardware_concurrency / 2 個ぶんくらいもつ
};

}// namespace cges::multithread