#pragma once

#include "renderbuffer.hpp"
#include "camera.hpp"
#include "renderer_base.hpp"

#include <memory>
#include <GLFW/glfw3.h>

namespace cges::gl {

// Graphics APIのラッパークラス 
// 複数ウィンドウは非対応
class Engine {
public:
  Engine(const size_t windowWidth, const size_t windowHeight, const char* const windowTitle);
  ~Engine();

  void Update(Camera& camera, std::shared_ptr<renderer::Base>& renderer);
  void Draw(const RenderBuffer& renderTarget) const;
  bool ShouldTerminate() const noexcept;

private:
  GLFWwindow* m_window;
};

} // namespace gl::engine