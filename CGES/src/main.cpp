#include <cstdint>
#include <iostream>

#include <glm/vec3.hpp>

#include "render_buffer.hpp"
#include "renderer.hpp"
#include "camera.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto lookingPos = glm::vec3(0, 0, 0);
  auto camera = cges::Camera();
  camera.fov = 90;
  camera.pos = { 0.0, 0.0, -5.0 };
  camera.upward = { 0.0, 1.0, 0.0 };
  auto renderer = cges::Renderer(camera, lookingPos, renderTarget);
  renderer.Update();
  renderer.Draw();
}