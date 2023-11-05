#pragma once

#include "renderbuffer.hpp"
#include "scene.hpp"
#include "renderer_base.hpp"
#include "multithread_mutexrenderprogress.hpp"

#include <thread>
#include <vector>

namespace cges::multithread {

// 1. ctor�Ƃ���Renderer, Scene, ���Ɖ����H���󂯎��
// 2. exec �݂����Ȋ֐��� height width �Ԃ񃋁[�v�܂킷
// 3. ���[�v���Ń����o�ϐ��̃X���b�h������1px�Ԃ�̏�����n���Ă���Ă��炤(�K�X�C�e���[�^��mutex guard���Đi�߂Ă���)
// 4. �Ō�܂ł������犮��

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
  std::vector<std::thread> m_threads; // hardware_concurrency / 2 �Ԃ񂭂炢����
};

}// namespace cges::multithread