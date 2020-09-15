#include "camera.hpp"
#include "renderbuffer.hpp"
#include "renderer_phoneshader.hpp"
#include "renderer_pathtracer.hpp"
#include "glengine.hpp"
#include "scene.hpp"
#include "gameobject_sphere.hpp"
#include "gameobject_polygonalmesh.hpp"
#include "gameobject_plane.hpp"
#include "material_lambertian.hpp"

#include <embree3/rtcore.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto embreeDevice = rtcNewDevice(nullptr);
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 1.0f, 0.0f, 0.0f }, 3.0f, 130.0f);
  auto graphicsEngine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
  auto scene = cges::Scene(embreeDevice);
  auto renderer = cges::renderer::PhoneShader();

  scene.Add(cges::MakePolygonalMesh(embreeDevice, 
                                    {-0.5f, -0.25f, 0.0f}, 
                                    "bin/goat_filled.obj",
                                    { 64, 64, 255 }, 
                                    {0, 0, 0},
                                    cges::material::Lambertian())); //山羊のオブジェ
  scene.Add(cges::MakePlane(embreeDevice, 
                            { 0.0f, 0.0f, 0.0f }, 
                            { +3.0f, -3.0f, +3.0f }, 
                            { +3.0f, +3.0f, +3.0f }, 
                            { +3.0f, +3.0f, -3.0f }, 
                            { 255, 100, 100 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 右壁
  scene.Add(cges::MakePlane(embreeDevice, 
                            { 0.0f, 0.0f, 0.0f }, 
                            { -3.0f, -3.0f, -3.0f }, 
                            { +3.0f, -3.0f, -3.0f }, 
                            { +3.0f, +3.0f, -3.0f }, 
                            { 192, 192, 192 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 奥壁
  scene.Add(cges::MakePlane(embreeDevice,
                            { 0.0f, 0.0f, 0.0f },
                            { +3.0f, +3.0f, 3.0f },
                            { +3.0f, -3.0f, 3.0f },
                            { -3.0f, -3.0f, 3.0f },
                            { 192, 192, 192 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 手前壁
  scene.Add(cges::MakePlane(embreeDevice,
                            { 0.0f, 0.0f, 0.0f }, 
                            { -3.0f, +3.0f, +3.0f }, 
                            { -3.0f, -3.0f, +3.0f }, 
                            { -3.0f, -3.0f, -3.0f }, 
                            "bin/texstone.jpg",
                            //{ 100, 100, 255 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 左壁
  scene.Add(cges::MakePlane(embreeDevice,
                            { 0.0f, 0.0f, 0.0f },
                            { +3.0f, +3.0f, -3.0f },
                            { +3.0f, +3.0f, +3.0f },
                            { -3.0f, +3.0f, +3.0f },
                            { 192, 192, 192 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 上壁
  scene.Add(cges::MakePlane(embreeDevice, 
                            { 0.0f, 0.0f, 0.0f }, 
                            { +3.0f, -3.0f, +3.0f }, 
                            { +3.0f, -3.0f, -3.0f }, 
                            { -3.0f, -3.0f, -3.0f }, 
                            { 192, 192, 192 },
                            { 0, 0, 0 },
                            cges::material::Lambertian())); // 下壁
  scene.Add(cges::MakeSphere(embreeDevice, 
                             { 1.5f, -1.5f, 0.0f }, 
                             0.75f, 
                             {192, 192, 192},
                             { 0, 0, 0 },
                             cges::material::Lambertian())); // 白球
  scene.Add(cges::MakePlane(embreeDevice,
                            { 0.0f, 0.0f, 0.0f },
                            { -2.0f, +2.75f, +1.5f },
                            { +2.0f, +2.75f, +1.5f },
                            { +2.0f, +2.75f, -1.5f },
                            { 0, 0, 0 },
                            { 223, 223, 223 },
                            cges::material::Lambertian())); // 天井の光源板

  while (!graphicsEngine.ShouldTerminate()) {
    graphicsEngine.Update(camera);
    scene.Update();
    
    renderer.Draw(camera, renderTarget, scene);
    graphicsEngine.Draw(renderTarget);
  }

  rtcReleaseDevice(embreeDevice);
}