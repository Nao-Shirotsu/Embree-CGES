#include "camera.hpp"
#include "renderbuffer.hpp"
#include "renderer_phoneshader.hpp"
#include "renderer_pathtracer.hpp"
#include "glengine.hpp"
#include "scene.hpp"
#include "gameobject_sphere.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_plane.hpp"

#include <embree3/rtcore.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto embreeDevice = rtcNewDevice(nullptr);
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0, 0, 0 }, 40.0f, 90.0f);
  auto graphicsEngine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
  auto scene = cges::Scene(embreeDevice);
  auto renderer = cges::renderer::PathTracer();

  scene.Add(cges::MakePolygonalMesh(embreeDevice, {0.0f, 0.0f, 0.0f}, "bin/goat_filled.obj", { 64, 64, 255 }));
  scene.Add(cges::MakePlane(embreeDevice, { 0.0f, 0.0f, 0.0f }, { +3.0f, -3.0f, +3.0f }, { +3.0f, -3.0f, -3.0f }, { +3.0f, +3.0f, -3.0f }, { 255, 64, 64 }));   // âEï«
  scene.Add(cges::MakePlane(embreeDevice, { 0.0f, 0.0f, 0.0f }, { -3.0f, -3.0f, -3.0f }, { +3.0f, -3.0f, -3.0f }, { +3.0f, +3.0f, -3.0f }, { 192, 192, 192 } ));// âúï«
  scene.Add(cges::MakePlane(embreeDevice, { 0.0f, 0.0f, 0.0f }, { -3.0f, -3.0f, +3.0f }, { -3.0f, -3.0f, -3.0f }, { -3.0f, +3.0f, -3.0f }, "bin/textest.jpg")); // ç∂ï«
  scene.Add(cges::MakePlane(embreeDevice, { 0.0f, 0.0f, 0.0f }, { -3.0f, +3.0f, +3.0f }, { +3.0f, +3.0f, +3.0f }, { +3.0f, +3.0f, -3.0f }, { 192, 192, 192 })); // è„ï«
  scene.Add(cges::MakePlane(embreeDevice, { 0.0f, 0.0f, 0.0f }, { +3.0f, -3.0f, +3.0f }, { +3.0f, -3.0f, -3.0f }, { -3.0f, -3.0f, -3.0f }, { 192, 192, 192 })); // â∫ï«
  scene.Add(cges::MakeSphere(embreeDevice, { 1.5f, -1.5f, 1.5f }, 1.0f, {0, 255, 0, 0}));

  while (!graphicsEngine.ShouldTerminate()) {
    graphicsEngine.Update(camera);
    scene.Update();
    renderer.Draw(camera, renderTarget, scene);
    graphicsEngine.Draw(renderTarget);
  }

  rtcReleaseDevice(embreeDevice);
}