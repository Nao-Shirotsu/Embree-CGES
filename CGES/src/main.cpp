#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#include "gl_engine.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera();
  auto renderer = cges::Renderer(camera, renderTarget);

  auto engine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Test Window");
  while(!engine.ShouldTerminate()) {
    engine.Update(camera);
    renderer.Update();
    renderer.Draw();
    engine.Draw(renderTarget);
  }
}

// TODO:
// - GameObject::LoadObjFileで、マーカー"v"と"f"以外も対応するように実装する
// - Renderer::Updateのレイトレ処理を高速化する
// - gl::Engine::Updateでfps制御と垂直同期をする
// - DXRデモのようにノイズをフレームごとに徐々に減らす描画にする
// - openglのバージョン指定を固定でなくする