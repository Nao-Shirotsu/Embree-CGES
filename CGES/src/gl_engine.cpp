#include "gl_engine.hpp"

#include "render_buffer.hpp"

#include <cassert>

namespace {

constexpr float DELTA_MOVE_AMOUNT = 0.125;
//constexpr float DELTA_MOVE_AMOUNT = 0.0625;

}

namespace cges::gl {

Engine::Engine(const size_t windowWidth, const size_t windowHeight, const char* const windowTitle) {
  if (glfwInit() == GL_FALSE) {
    assert(false);
  }
  m_window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
  if (!m_window) {
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1);

  m_valid = true;
}

Engine::~Engine() {
  glfwTerminate();
}

void Engine::Update(Camera& camera) {
  if (glfwGetKey(m_window, GLFW_KEY_UP)) {
    camera.pos.y += DELTA_MOVE_AMOUNT;
    camera.lookingPos.y += DELTA_MOVE_AMOUNT;
  }
  if (glfwGetKey(m_window, GLFW_KEY_RIGHT)) {
    camera.pos.x += DELTA_MOVE_AMOUNT;
    camera.lookingPos.x += DELTA_MOVE_AMOUNT;
  }
  if (glfwGetKey(m_window, GLFW_KEY_DOWN)) {
    camera.pos.y -= DELTA_MOVE_AMOUNT;
    camera.lookingPos.y -= DELTA_MOVE_AMOUNT;
  }
  if (glfwGetKey(m_window, GLFW_KEY_LEFT)) {
    camera.pos.x -= DELTA_MOVE_AMOUNT;
    camera.lookingPos.x -= DELTA_MOVE_AMOUNT;
  }
}

void Engine::Draw(const RenderBuffer& renderTarget) const {
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(renderTarget.GetWidth(), renderTarget.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, renderTarget[0]);
  glFlush();
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

bool Engine::ShouldTerminate() const noexcept {
  return m_valid && glfwWindowShouldClose(m_window);
}

} // namespace gl::enginea