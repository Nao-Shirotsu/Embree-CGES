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

  int verBufIndex = 0, idxBufIndex = 0;
  do {
    if (ifs.peek() == '#') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      continue;
    }
    ifs >> marker;
    switch (obj::ToMarker(marker)) {
      case obj::Marker::V: {
        std::string recv;
        ifs >> recv;
        verBuf[verBufIndex].x = std::stof(recv);
        ifs >> recv;
        verBuf[verBufIndex].y = std::stof(recv);
        ifs >> recv;
        verBuf[verBufIndex].z = std::stof(recv);
        ++verBufIndex;
      } break;

      case obj::Marker::F: {
        std::string recv;
        ifs >> recv;
        idxBuf[idxBufIndex].v0 = std::stoi(recv);
        ifs >> recv;
        idxBuf[idxBufIndex].v1 = std::stoi(recv);
        ifs >> recv;
        idxBuf[idxBufIndex].v2 = std::stoi(recv);
        ++idxBufIndex;
      } break;

      case obj::Marker::MTLLIB: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case obj::Marker::USEMTL: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case obj::Marker::G: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case obj::Marker::VN: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case obj::Marker::VT: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      default: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;
    }
  } while (!ifs.eof());

  rtcCommitGeometry(m_geometry);
  return true;
}

uint32_t GameObject::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_geometry);
}

} // namespace cges