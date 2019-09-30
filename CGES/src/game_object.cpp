#include "game_object.hpp"

#include "util_objfile.hpp"
#include "constants.hpp"

#include <fstream>
#include <string>
#include <cstdio>

#include <glm/vec3.hpp>

namespace cges {

GameObject::GameObject(const RTCDevice device)
    : m_geometry{ nullptr } {
  m_geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
}

GameObject::~GameObject() {
  rtcReleaseGeometry(m_geometry);
}


bool GameObject::LoadObjFile(const char* const objFileName) {
  auto ifs = std::ifstream(objFileName);
  if (!ifs) {
    return false;
  }
  obj::NumElements count;
  if (!obj::CountCategories(ifs, count)) {
    return false;
  }

  auto verBuf = reinterpret_cast<PolygonVertices*>(
      rtcSetNewGeometryBuffer(
          m_geometry,
          RTC_BUFFER_TYPE_VERTEX,
          0,
          RTC_FORMAT_FLOAT3,
          sizeof(PolygonVertices),
          count[obj::Marker::V]));

  auto idxBuf = reinterpret_cast<PolygonIndices*>(
      rtcSetNewGeometryBuffer(
          m_geometry,
          RTC_BUFFER_TYPE_INDEX,
          0,
          RTC_FORMAT_UINT3,
          sizeof(PolygonIndices),
          count[obj::Marker::F]));

  if (!verBuf || !idxBuf) {
    return false;
  }

  std::string marker;
  char dummy[INPUT_BUFFER_SIZE];

  for (int verBufIndex = 0, idxBufIndex = 0; !ifs.eof();) {
    if (ifs.peek() == '#') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1çsîÚÇŒÇ∑
      continue;
    }
    ifs >> marker;
    switch (obj::ToMarker(marker)) {
      case obj::Marker::V: {
        ifs >> verBuf[verBufIndex].x;
        ifs >> verBuf[verBufIndex].y;
        ifs >> verBuf[verBufIndex].z;
        ++verBufIndex;
      } break;

      case obj::Marker::F: {
        ifs >> idxBuf[idxBufIndex].v0;
        ifs >> idxBuf[idxBufIndex].v1;
        ifs >> idxBuf[idxBufIndex].v2;
        ++idxBufIndex;
      } break;

      default: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1çsîÚÇŒÇ∑
      } break;
    }
  }

  rtcCommitGeometry(m_geometry);
  return true;
}

uint32_t GameObject::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_geometry);
}

} // namespace cges