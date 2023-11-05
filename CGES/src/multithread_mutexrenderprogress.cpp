#include "multithread_mutexrenderprogress.hpp"

namespace cges::multithread {

MutexRenderProgress::MutexRenderProgress(const RenderBuffer& renderTarget) noexcept
    : m_renderBufferWidth(renderTarget.GetWidth())
    , m_renderProgressEndIdx(renderTarget.GetHeight() * m_renderBufferWidth){}

MutexRenderProgress& MutexRenderProgress::operator++() noexcept {
  std::lock_guard<std::mutex> lock(m_idxMutex);
  ++m_renderProgressIdx;
  return *this;
}

void MutexRenderProgress::GetNextIdx(unsigned int& h, unsigned int& w) noexcept {
  std::lock_guard<std::mutex> lock(m_idxMutex);
  h = m_renderProgressIdx / m_renderBufferWidth;
  w = m_renderProgressIdx % m_renderBufferWidth;
}

bool MutexRenderProgress::Completed() noexcept {
  std::lock_guard<std::mutex> lock(m_idxMutex);
  return m_renderProgressIdx >= m_renderProgressEndIdx;
}

} // namespace cges::multithread