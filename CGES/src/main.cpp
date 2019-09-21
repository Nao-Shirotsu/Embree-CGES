#include <cstdint>
#include <iostream>
#include <numeric>

#include <GLFW/glfw3.h>
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include <glm/vec4.hpp>

#include "object.hpp"
#include "render_buffer.hpp"

constexpr float INF = std::numeric_limits<float>::max();
constexpr int WINDOW_WIDTH = 16 * 50;
constexpr int WINDOW_HEIGHT = 9 * 50;
constexpr int FOV = 120;
constexpr int HALF_FOV = FOV / 2;

int main() {
  RTCDevice device = rtcNewDevice(nullptr);
  if (!device) {
    std::cout << "Device Error : " << rtcGetDeviceError(nullptr) << std::endl;
    return 0;
  }

  // ================= Init Scene  =================
  RTCScene scene = rtcNewScene(device);
  {
    auto object = shi::Object(device, scene);
    std::cout << std::boolalpha << object.LoadObjFile("tetra.obj") << std::endl;
    object.AttachTo(scene);
  }
  rtcCommitScene(scene);
  // ================= Init Scene  =================

  auto renderTarget = shi::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  

  // ================= Ray Casting  =================
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCRayHit rayhit;
  rayhit.ray.org_x = 0;
  rayhit.ray.org_y = -1.3;
  rayhit.ray.org_z = -3;
  rayhit.ray.dir_x = 0.0;
  rayhit.ray.dir_y = 0.0;
  rayhit.ray.dir_z = 1;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = INF;
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

  for (int i = 0; i < 22; ++i) {
    auto testRay = rayhit;
    testRay.ray.org_y += i * 0.2;
    rtcIntersect1(scene, &context, &testRay);
    if (testRay.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
      std::cout << "Intersected";
    }
    else {
      std::cout << "No intersection";
    }
    std::cout << " at y=" << testRay.ray.org_y << std::endl;
  }
  // ================= Ray Casting  =================

  rtcReleaseScene(scene);
  rtcReleaseDevice(device);
}




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
    std::cout << std::boolalpha << object.LoadObjFile("tetra.obj") << std::endl;
    object.AttachTo(scene);
  }
  rtcCommitScene(scene);
  // ================= Init Scene  =================

  // ================= Ray Casting  =================
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCRayHit rayhit;
  rayhit.ray.org_x = 0;
  rayhit.ray.org_y = -1.3;
  rayhit.ray.org_z = -3;
  rayhit.ray.dir_x = 0.0;
  rayhit.ray.dir_y = 0.0;
  rayhit.ray.dir_z = 1;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = INF;
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

  for (int i = 0; i < 22; ++i) {
    auto testRay = rayhit;
    testRay.ray.org_y += i * 0.2;
    rtcIntersect1(scene, &context, &testRay);
    if (testRay.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
      std::cout << "Intersected";
    }
    else {
      std::cout << "No intersection";
    }
    std::cout << " at y=" << testRay.ray.org_y << std::endl;
  }
  // ================= Ray Casting  =================

  rtcReleaseScene(scene);
  rtcReleaseDevice(device);
}