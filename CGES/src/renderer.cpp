#include "renderer.hpp"

#include <glm/glm.hpp>

#ifdef _DEBUG
#include <iostream>
#endif

namespace {

constexpr float SCREEN_WIDTH = 0.25;

}

namespace cges {

Renderer::Renderer(const Camera& camera, const glm::vec3& lookingPos, RenderBuffer& renderTarget)
    : m_rtcDevice{ rtcNewDevice(nullptr) }
    , m_rtcScene{ rtcNewScene(m_rtcDevice) }
    , m_camera{ camera }
    , m_renderTarget{ renderTarget }
    , m_lookingPos{ lookingPos } {
  auto object = cges::GameObject(m_rtcDevice);
  assert(object.LoadObjFile("bunny.obj"));
  object.AttachTo(m_rtcScene);
  rtcCommitScene(m_rtcScene);
}

Renderer::~Renderer() {
  rtcReleaseScene(m_rtcScene);
  rtcReleaseDevice(m_rtcDevice);
}

void Renderer::Update() {
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);

  const int32_t width = static_cast<int32_t>(m_renderTarget.GetWidth());
  const int32_t height = static_cast<int32_t>(m_renderTarget.GetHeight());
  const float aspectRatio = height / static_cast<float>(width);
  const glm::vec3 cameraFront = glm::normalize(m_lookingPos - m_camera.pos);
  const glm::vec3 screenHorizontalVec = glm::normalize(glm::cross(m_camera.upward, cameraFront)) * SCREEN_WIDTH;
  const glm::vec3 screenVerticalVec = glm::normalize(glm::cross(screenHorizontalVec, cameraFront)) * SCREEN_WIDTH * aspectRatio;
  const glm::vec3 screenCenterPos = m_camera.pos + cameraFront;

  // スクリーン左上(pixel[0][0])のワールド座標
  const glm::vec3 initialPos = screenCenterPos - (glm::vec3{ 0.5, 0.5, 0.5 } * screenVerticalVec) - (glm::vec3{ 0.5, 0.5, 0.5 } * screenHorizontalVec);

  for (int y = 0; y < height; ++y) {
    const float yRate = y / static_cast<float>(height);
#ifdef _DEBUG
    std::cout << "rendering... (" << static_cast<int>(yRate * 100) << "%)" << std::endl;
#endif
    for (int x = 0; x < width; ++x) {
      const float xRate = x / static_cast<float>(width);
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 rayDir = pixelPos - m_camera.pos;
      RTCRayHit rayhit;
      rayhit.ray.org_x = m_camera.pos.x;
      rayhit.ray.org_y = m_camera.pos.y;
      rayhit.ray.org_z = m_camera.pos.z;
      rayhit.ray.dir_x = rayDir.x;
      rayhit.ray.dir_y = rayDir.y;
      rayhit.ray.dir_z = rayDir.z;
      rayhit.ray.tnear = 0;
      rayhit.ray.tfar = 100000000; // 適当
      rayhit.ray.mask = 0;
      rayhit.ray.flags = 0;
      rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rtcIntersect1(m_rtcScene, &context, &rayhit);
      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        m_renderTarget[y][x].r = 255;
        m_renderTarget[y][x].b = 255;
      }
    }
  }
}

void cges::Renderer::Draw() const {
  m_renderTarget.SaveAsPpm("out.ppm");
}

} // namespace cges::obj