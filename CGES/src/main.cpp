#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto lookingPos = glm::vec3(0, 0.0, 0);
  auto camera = cges::Camera();
  camera.fov = 90;
  camera.pos = { 0.0, 0.0, -5.0 };
  camera.upward = { 0.0, 1.0, 0.0 };
  auto renderer = cges::Renderer(camera, lookingPos, renderTarget);
  renderer.Update();
  renderer.Draw();
}

// TODO:
// - GameObject::LoadObjFileで、マーカー"v"と"f"以外も対応するように実装する
