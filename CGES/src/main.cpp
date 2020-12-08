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
#include <memory>
#include <chrono>
#include <string>
#include <iostream>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

class StopWatch {
public:
  StopWatch() noexcept
      : start()
      , elapsedTime(std::chrono::microseconds(0)) {}

  void Start() noexcept {
    start = std::chrono::system_clock::now();
  }

  void Stop() noexcept {
    using namespace std::chrono;
    elapsedTime += duration_cast<microseconds>(system_clock::now() - start);
  }

  void Reset() noexcept {
    using namespace std::chrono;
    elapsedTime = microseconds(0);
  }

  unsigned int GetMicroseconds() const noexcept {
    using namespace std::chrono;
    return static_cast<unsigned int>(elapsedTime.count());
  }

private:
  std::chrono::system_clock::time_point start;
  std::chrono::microseconds elapsedTime;
};

int main() {
  auto embreeDevice = rtcNewDevice(nullptr);
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0.0f, 0.0f, 0.0f }, 3.0f, 130.0f);
  auto graphicsEngine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto scene = cges::Scene(embreeDevice);
  std::shared_ptr<cges::renderer::Base> renderer = std::make_shared<cges::renderer::PhongShader>();

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

  /*while (!graphicsEngine.ShouldTerminate()) {
    graphicsEngine.Update(camera, renderer);
    scene.Update();
    renderer->Update(camera);
    renderer->Draw(camera, renderTarget, scene);
    graphicsEngine.Draw(renderTarget);
  }*/

  StopWatch timer;

  scene.Update();
  
  auto path = cges::renderer::PathTracer(5, 32, 1024);
  timer.Start();
  path.Draw(camera, renderTarget, scene);
  timer.Stop();
  std::cout << timer.GetMicroseconds() << std::endl;
  auto filename = std::string("path") + std::to_string(timer.GetMicroseconds()) + std::string(".ppm");
  renderTarget.SaveAsPpm(filename.c_str());

  timer.Reset();

  rtcReleaseDevice(embreeDevice);
}