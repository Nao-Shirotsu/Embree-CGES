#include "glengine.hpp"

#include "renderbuffer.hpp"

#include <algorithm>
#include <cassert>
#include <glm/glm.hpp>

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float DELTA_SPIN_RADIAN = PI / 15.0f;

}

namespace cges::gl {

Engine::Engine(const size_t windowWidth, const size_t windowHeight, const char* const windowTitle) {
  if (glfwInit() == GL_FALSE) {
    assert(false);
  }
  m_window = glfwCreateWindow(static_cast<int>(windowWidth), static_cast<int>(windowHeight), windowTitle, nullptr, nullptr);
  if (!m_window) {
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

  glfwMakeContextCurrent(m_window);
  glfwSwapInterval(1);
}

Engine::~Engine() {
  glfwTerminate();
}

void Engine::Update(Camera& camera) {
  if (camera.JustMoved()) {
    camera.StayPosition();
  }

  if (glfwGetKey(m_window, GLFW_KEY_UP)) {
    camera.radYZ = std::clamp(camera.radYZ - DELTA_SPIN_RADIAN, DELTA_SPIN_RADIAN, PI - DELTA_SPIN_RADIAN);
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_RIGHT)) {
    camera.radXZ += DELTA_SPIN_RADIAN;
    if (camera.radXZ > 2.0f * PI) {
      camera.radXZ = DELTA_SPIN_RADIAN;
    }
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_DOWN)) {
    camera.radYZ = std::clamp(camera.radYZ + DELTA_SPIN_RADIAN, DELTA_SPIN_RADIAN, PI - DELTA_SPIN_RADIAN);
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_LEFT)) {
    camera.radXZ -= DELTA_SPIN_RADIAN;
    if (camera.radXZ < DELTA_SPIN_RADIAN) {
      camera.radXZ = 2.0f * PI;
    }
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_I)) {
    camera.radius -= 0.5f;
    if (camera.radius < 0.0f) {
      camera.radius = 0.0f;
    }
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_O)) {
    camera.radius += 0.5f;
    camera.UpdatePosLocal();
  }
  if (glfwGetKey(m_window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(m_window, static_cast<int>(true));
  }
}

void Engine::Draw(const RenderBuffer& renderTarget) const {
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(static_cast<int>(renderTarget.GetWidth()), static_cast<int>(renderTarget.GetHeight()), GL_RGBA, GL_UNSIGNED_BYTE, renderTarget[0]);
  glFlush();
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

bool Engine::ShouldTerminate() const noexcept {
  return glfwWindowShouldClose(m_window);
}

} // namespace gl::enginea