#include "glengine.hpp"

#include "renderbuffer.hpp"
#include "renderer_phoneshader.hpp"
#include "renderer_pathtracer.hpp"
#include "renderer_neept.hpp"

#include <algorithm>
#include <cassert>
#include <glm/glm.hpp>
#include <iostream>

namespace {

constexpr float PI = 3.14159265358979323846f;
constexpr float DELTA_SPIN_RADIAN = PI / 15.0f;

}

namespace cges::gl {

Engine::Engine(const size_t windowWidth, const size_t windowHeight) {
  if (glfwInit() == GL_FALSE) {
    assert(false);
  }
  m_window = glfwCreateWindow(static_cast<int>(windowWidth), static_cast<int>(windowHeight), "CGES [Phong Shading]", nullptr, nullptr);
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

void Engine::Update(Camera& camera, std::shared_ptr<renderer::Base>& renderer) {
  if (camera.JustMoved()) {
    camera.StayPosition();
  }

  // ======レンダラ切り替え======
  if (glfwGetKey(m_window, GLFW_KEY_1) && renderer->RenderMethod() != renderer::Method::PhongShading) {
    glfwSetWindowTitle(m_window, "CGES [Phong Shading]");
    renderer = std::make_shared<renderer::PhongShader>();
  }

  if (glfwGetKey(m_window, GLFW_KEY_2) && renderer->RenderMethod() != renderer::Method::Pathtracing) {
    glfwSetWindowTitle(m_window, "CGES [Pathtracing]");
    renderer = std::make_shared<cges::renderer::PathTracer>(5, 64, 512);
  }

  if (glfwGetKey(m_window, GLFW_KEY_3) && renderer->RenderMethod() != renderer::Method::NEEPathtracing) {
    glfwSetWindowTitle(m_window, "CGES [Pathtracing with Next Event Estimation]");
    renderer = std::make_shared<cges::renderer::NEEPathTracer>(5, 64, 512);
  }

  // ======カメラ移動入力======
  if (glfwGetKey(m_window, GLFW_KEY_UP)) {
    camera.radYZ = std::clamp(camera.radYZ - DELTA_SPIN_RADIAN, DELTA_SPIN_RADIAN, PI - DELTA_SPIN_RADIAN);
    camera.UpdatePosLocal();
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

  // ======ゲーム終了======
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

bool Engine::InterruptionRequested() const noexcept {
  return false;
}

} // namespace gl::enginea