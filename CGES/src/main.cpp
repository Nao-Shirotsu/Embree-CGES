#include <iostream>
#include <cstdint>
#include <numeric>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

constexpr float INF = std::numeric_limits<float>::max();

int main() {
  RTCDevice device = rtcNewDevice(nullptr);
  if(!device) {
    std::cout << "Device Error : " << rtcGetDeviceError(nullptr) << std::endl;
  }

  // ================= Init Scene  =================
  RTCScene scene = rtcNewScene(device);
  RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  float* vertices = static_cast<float*>(rtcSetNewGeometryBuffer(geometry,
                                                                RTC_BUFFER_TYPE_VERTEX,
                                                                0,
                                                                RTC_FORMAT_FLOAT3,
                                                                3 * sizeof(float),
                                                                3));
  uint32_t* indices = static_cast<unsigned*>(rtcSetNewGeometryBuffer(geometry,
                                                                     RTC_BUFFER_TYPE_INDEX,
                                                                     0,
                                                                     RTC_FORMAT_UINT3,
                                                                     3 * sizeof(uint32_t),
                                                                     1));
  
  if (vertices && indices) {
    vertices[0] = 0.f;
    vertices[1] = 0.f;
    vertices[2] = 0.f;
    vertices[3] = 1.f;
    vertices[4] = 0.f;
    vertices[5] = 0.f;
    vertices[6] = 0.f;
    vertices[7] = 1.f;
    vertices[8] = 0.f;
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
  }
  rtcCommitGeometry(geometry);
  rtcAttachGeometry(scene, geometry);
  rtcReleaseGeometry(geometry);
  rtcCommitScene(scene);
  // ================= Init Scene  =================

  // ================= Ray Casting  =================
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCRayHit rayhit;
  rayhit.ray.org_x = 0;
  rayhit.ray.org_y = 0;
  rayhit.ray.org_z = -1;
  rayhit.ray.dir_x = 0;
  rayhit.ray.dir_y = 0;
  rayhit.ray.dir_z = 1;
  rayhit.ray.tnear = 0;
  rayhit.ray.tfar = INF;
  rayhit.ray.mask = 0;
  rayhit.ray.flags = 0;
  rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
  rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
  
  rtcIntersect1(scene, &context, &rayhit);
  if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
    std::cout << "Intersected at" << rayhit.ray.tfar << std::endl;
  }
  else {
    std::cout << "Not intersected" << std::endl;
  }
  // ================= Ray Casting  =================

  rtcReleaseScene(scene);
  rtcReleaseDevice(device);
}