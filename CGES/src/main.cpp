#include "camera.hpp"
#include "renderbuffer.hpp"
#include "renderer.hpp"
#include "glengine.hpp"
#include "scene.hpp"
#include "gameobject_sphere.hpp"
#include "gameobject_polygonalmesh.hpp"

#include <embree3/rtcore.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto embreeDevice = rtcNewDevice(nullptr);
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0, 0, 0 }, 40.0f);
  auto graphicsEngine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
  auto scene = cges::Scene(embreeDevice);
  auto renderer = cges::Renderer();

  //scene.Add(cges::MakePolygonalMesh(embreeDevice, "bin/goat_filled.obj", {64, 64, 255}));
  scene.Add(cges::MakeSphere(embreeDevice, 1.25f, "bin/aizu_library.jpg"));

  while (!graphicsEngine.ShouldTerminate()) {
    graphicsEngine.Update(camera);
    scene.Update();
    renderer.Draw(camera, renderTarget, scene);
    graphicsEngine.Draw(renderTarget);
  }

  rtcReleaseDevice(embreeDevice);
}

// TODO:
// - gameobject::PolygonModel::LoadObjFileで、マーカー"v"と"f"以外も対応するように実装する
// - Renderer::Updateのレイトレ処理を高速化する
// - gl::Engine::Updateでfps制御と垂直同期をする
// - DXRデモのようにノイズをフレームごとに徐々に減らす描画にする
// - openglのバージョン指定を固定でなくする
// - gameobject::Baseを三角形以外にも対応させる(言うてgridだけでよさそう)
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