#include <cstdint>
#include <iostream>
#include <numeric>

#include <GLFW/glfw3.h>
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include <glm/vec4.hpp>

#include "object.hpp"
#include "render_buffer.hpp"

void ExecEmbreeRayhit();
void ExecOpenglProcess();

int main() {
  //ExecEmbreeRayhit();
  //ExecOpenglProcess();
  constexpr int X = 100;
  constexpr int Y = 100;
  auto image = shi::RenderBuffer(100, 100);
  for(int i = 0; i < Y; ++i) {
    for(int j = 0; j < X; ++j) {
      image[i][j] = {0, static_cast<uint8_t>(2 * i), static_cast<uint8_t>(2 * j), 0 };
    }
  }
  std::cout << std::boolalpha << image.SaveAsPpm("test.ppm") << std::endl;
}

constexpr float INF = std::numeric_limits<float>::max();

void ExecOpenglProcess() {
  if (glfwInit() == GL_FALSE) {
    std::cout << "GLFW init failed" << std::endl;
    return;
  }
  GLFWwindow* window = glfwCreateWindow(500, 500, "Test Window", nullptr, nullptr);
  if (!window) {
    std::cout << "GL window creation failed" << std::endl;
    glfwTerminate();
    return;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  while (glfwWindowShouldClose(window) == GL_FALSE) {
    glm::vec4{ 1, 2, 3, 4 };
    glClearColor(0.25, 0.25, 0.25, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
}

void ExecEmbreeRayhit() {
  RTCDevice device = rtcNewDevice(nullptr);
  if (!device) {
    std::cout << "Device Error : " << rtcGetDeviceError(nullptr) << std::endl;
  }

  // ================= Init Scene  =================
  RTCScene scene = rtcNewScene(device);
  {
    auto object = shi::Object(device, scene);
    object.LoadObjFile("hogehoge.obj");
    object.AttachTo(scene);
  }
  rtcCommitScene(scene);
  // ================= Init Scene  =================

  // ================= Ray Casting  =================
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCRayHit rayhit;
  rayhit.ray.org_x = 0.25;
  rayhit.ray.org_y = 0.25;
  rayhit.ray.org_z = -1;
  rayhit.ray.dir_x = 0.25;
  rayhit.ray.dir_y = 0.25;
  rayhit.ray.dir_z = 1;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = INF;
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

  rtcIntersect1(scene, &context, &rayhit);
  if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    std::cout << "Intersected at" << rayhit.ray.tfar << std::endl;
  }
  else {
    std::cout << "No intersection" << std::endl;
  }
  // ================= Ray Casting  =================

  rtcReleaseScene(scene);
  rtcReleaseDevice(device);
}