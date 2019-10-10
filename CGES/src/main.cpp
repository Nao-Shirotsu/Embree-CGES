#include "camera.hpp"
#include "render_buffer.hpp"
#include "renderer.hpp"
#include "gl_engine.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0.0f, 0.25f, -1.5f }, 40.0f);
  auto renderer = cges::Renderer(camera, renderTarget);

  auto engine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
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
// - GameObjectを三角形以外にも対応させる(言うてgridだけでよさそう)
// - SSE4, AVX対応を調べてintersect1~intersect4などを切り替える

// effect実装案
// - 半透明
// - GI
// - reflection
// - テクスチャマッピング
// - トゥーンシェーディング
// - グーローシェーディング
// - ブラー
// - バンプマッピング