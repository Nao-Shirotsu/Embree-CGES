#include <array>
#include <vector>
#include <embree3/rtcore.h>
#include <iostream>
#include <limits>
#include <fstream>

struct Vertex {
  float x, y, z;
};

struct PolygonIndex {
  unsigned int v0, v1, v2;
};

namespace {
constexpr uint32_t SCREEN_WIDTH_PX = 100;
constexpr uint32_t SCREEN_HEIGHT_PX = 100;
}

struct ColorRGB {
  uint8_t r, g, b;
};

//void SaveAsPpm(const std::array<std::array<ColorRGB, SCREEN_WIDTH_PX>, SCREEN_HEIGHT_PX>& image) {
void SaveAsPpm(const std::vector<std::array<ColorRGB, SCREEN_WIDTH_PX>>& image) {
  std::ofstream ofs("result.ppm");

  if (!ofs) {
    std::exit(1);
  }

  ofs << "P3\n"
      << SCREEN_WIDTH_PX << " " << SCREEN_HEIGHT_PX << "\n255\n";
  for (const auto& row : image) {
    for (const auto& pixel : row) {
      ofs << static_cast<int>(pixel.r) << " "
          << static_cast<int>(pixel.g) << " "
          << static_cast<int>(pixel.b) << "\n";
    }
  }

  ofs.close();
}

int main() {
  RTCDevice deviceHandle = rtcNewDevice(nullptr);
  RTCScene sceneHandle = rtcNewScene(deviceHandle);
  RTCGeometry geometryHandle = rtcNewGeometry(deviceHandle, RTC_GEOMETRY_TYPE_TRIANGLE);

  constexpr int numVertices = 6;
  constexpr int numPolygons = 8;

  //Vertex* geometryVertices = reinterpret_cast<Vertex*>(rtcSetNewGeometryBuffer(geometryHandle,
  //                                                                             RTC_BUFFER_TYPE_VERTEX,
  //                                                                             0,
  //                                                                             RTC_FORMAT_FLOAT3,
  //                                                                             sizeof(Vertex),
  //                                                                             numVertices));
  //PolygonIndex* geometryPolygons = reinterpret_cast<PolygonIndex*>(rtcSetNewGeometryBuffer(geometryHandle,
  //                                                                        RTC_BUFFER_TYPE_INDEX,
  //                                                                        0,
  //                                                                        RTC_FORMAT_UINT3,
  //                                                                        sizeof(PolygonIndex),
  //                                                                        numPolygons));

  std::array<Vertex, numVertices> geometryVertices;
  std::array<PolygonIndex, numPolygons> geometryPolygons;

  rtcSetSharedGeometryBuffer(geometryHandle,
                             RTC_BUFFER_TYPE_VERTEX,
                             0,
                             RTC_FORMAT_FLOAT3,
                             &geometryVertices,
                             0,
                             sizeof(Vertex),
                             geometryVertices.size());

  geometryVertices[0] = { -0.5f,  0.0f,  0.0f };
  geometryVertices[1] = {  0.0f,  1.0f,  0.0f };
  geometryVertices[2] = {  0.5f,  0.0f,  0.0f };
  geometryVertices[3] = {  0.0f, -1.0f,  0.0f };
  geometryVertices[4] = {  0.0f,  0.0f, -0.5f };
  geometryVertices[5] = {  0.0f,  0.0f,  0.5f };

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
  std::vector<std::array<ColorRGB, SCREEN_WIDTH_PX>> screen{SCREEN_HEIGHT_PX};

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

  return 0;
}