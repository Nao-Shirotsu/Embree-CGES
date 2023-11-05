#pragma once

#include "renderbuffer.hpp"

#include <mutex>

namespace cges::multithread {

// �X�P�W���[������RendererNuffer�̐i���Ǘ��C�e���[�^�̂悤�Ɏg��
class MutexRenderProgress {
public:
  MutexRenderProgress(const RenderBuffer& renderTarget) noexcept;

  MutexRenderProgress& operator++() noexcept;

  void GetNextIdx(unsigned int& h, unsigned int& w) noexcept;

  bool Completed() noexcept;

private:
  unsigned int m_renderProgressIdx = 0;
  const unsigned int m_renderBufferWidth;
  const unsigned int m_renderProgressEndIdx; // �`��i���I����idx(renderBuffer��h*w�ɂȂ邾�낤)
  std::mutex m_idxMutex;
};

} // namespace cges::multithread