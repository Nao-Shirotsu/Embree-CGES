#pragma once

#include "renderbuffer.hpp"

#include <mutex>

namespace cges::multithread {

// スケジューラ内でRendererNufferの進捗管理イテレータのように使う
class MutexRenderProgress {
public:
  MutexRenderProgress(const RenderBuffer& renderTarget) noexcept;

  MutexRenderProgress& operator++() noexcept;

  void GetNextIdx(unsigned int& h, unsigned int& w) noexcept;

  bool Completed() noexcept;

private:
  unsigned int m_renderProgressIdx = 0;
  const unsigned int m_renderBufferWidth;
  const unsigned int m_renderProgressEndIdx; // 描画進捗終了のidx(renderBufferのh*wになるだろう)
  std::mutex m_idxMutex;
};

} // namespace cges::multithread