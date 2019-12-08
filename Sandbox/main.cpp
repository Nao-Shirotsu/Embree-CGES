#include <embree3/rtcore.h>

struct SphereBuffer {
  float radius;
};

int main() {
  RTCDevice deviceHandle = rtcNewDevice(nullptr);
  RTCScene sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_SPHERE_POINT);

  

  rtcSetSharedGeometryBuffer(geometryHandle,
    RTC_BUFFER_TYPE_VERTEX,
    0,
    RTC_FORMAT_FLOAT3,
    &geometryVertices,
    0,
    sizeof(Vertex),
    geometryVertices.size());

  geometryVertices[0] = { -0.5f,  0.0f,  0.0f };
  geometryVertices[1] = { 0.0f,  1.0f,  0.0f };
  geometryVertices[2] = { 0.5f,  0.0f,  0.0f };
  geometryVertices[3] = { 0.0f, -1.0f,  0.0f };
  geometryVertices[4] = { 0.0f,  0.0f, -0.5f };
  geometryVertices[5] = { 0.0f,  0.0f,  0.5f };

  rtcSetSharedGeometryBuffer(geometryHandle,
    RTC_BUFFER_TYPE_INDEX,
    0,
    RTC_FORMAT_UINT3,
    &geometryPolygons,
    0,
    sizeof(PolygonIndex),
    geometryPolygons.size());

  geometryPolygons[0] = { 1, 2, 5 };
  geometryPolygons[1] = { 2, 3, 5 };
  geometryPolygons[2] = { 3, 4, 5 };
  geometryPolygons[3] = { 4, 1, 5 };
  geometryPolygons[4] = { 2, 1, 6 };
  geometryPolygons[5] = { 3, 2, 6 };
  geometryPolygons[6] = { 4, 3, 6 };
  geometryPolygons[7] = { 1, 4, 6 };

  rtcCommitGeometry(geometryHandle);
  rtcAttachGeometry(sceneHandle, geometryHandle);
  rtcReleaseGeometry(geometryHandle);
  rtcCommitScene(sceneHandle);

  constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();
  std::vector<std::array<ColorRGB, SCREEN_WIDTH_PX>> screen{ SCREEN_HEIGHT_PX };

  constexpr int HALF_HEIGHT = static_cast<int>(SCREEN_HEIGHT_PX) / 2;
  constexpr int HALF_WIDTH = static_cast<int>(SCREEN_WIDTH_PX) / 2;
  uint32_t count = 0;
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  for (int y = -HALF_HEIGHT; y < HALF_HEIGHT; ++y) {
    for (int x = -HALF_WIDTH; x < HALF_WIDTH; ++x) {
      RTCRayHit rayhit;
      rayhit.ray.org_x = x / static_cast<float>(HALF_WIDTH);
      rayhit.ray.org_y = y / static_cast<float>(HALF_HEIGHT);
      rayhit.ray.org_z = -5.0f;
      rayhit.ray.dir_x = 0.0f;
      rayhit.ray.dir_y = 0.0f;
      rayhit.ray.dir_z = 10.0f;
      rayhit.ray.tnear = 0;
      rayhit.ray.tfar = FLOAT_INFINITY;
      rayhit.ray.mask = 0;
      rayhit.ray.flags = 0;
      rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
      rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

      rtcIntersect1(sceneHandle, &context, &rayhit);
      const uint32_t yIdx = y + HALF_HEIGHT;
      const uint32_t xIdx = x + HALF_WIDTH;
      if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
        //std::cout << "Hit at (" << rayhit.ray.org_x << ", " << rayhit.ray.org_y << ")" << std::endl;
        screen[yIdx][xIdx].r = 0xff;
        screen[yIdx][xIdx].g = 0x00;
        screen[yIdx][xIdx].b = 0xff;
        ++count;
      }
      else {
        //std::cout << "No hit at" << rayhit.ray.org_x << ", " << rayhit.ray.org_y << ")" << std::endl;
        screen[yIdx][xIdx].r = 0x00;
        screen[yIdx][xIdx].g = 0x00;
        screen[yIdx][xIdx].b = 0x00;
      }
    }
  }

  SaveAsPpm(screen);

  rtcReleaseScene(sceneHandle);
  rtcReleaseDevice(deviceHandle);
}