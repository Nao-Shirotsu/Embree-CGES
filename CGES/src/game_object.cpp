#include "game_object.hpp"

#include "constants.hpp"
#include "util_objfile.hpp"

#include <cstdio>
#include <fstream>
#include <string>

#include <glm/vec3.hpp>

namespace cges {

GameObject::GameObject(const RTCDevice device)
    : m_device{ device }
    , m_geometry{ rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE) }
    , m_verBuf{ 0 }
    , m_idxBuf{ 0 } {
}

GameObject::GameObject(const RTCDevice device, const char* const filePath)
    : GameObject(device) {
  bool success = LoadObjFile(filePath);
  assert(success);
}

GameObject::GameObject(const GameObject& other)
    : m_device{ other.m_device }
    , m_geometry{ rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE) }
    , m_verBuf{ other.m_verBuf }
    , m_idxBuf{ other.m_idxBuf } {
}

GameObject::GameObject(GameObject&& other) noexcept
    : m_device{ other.m_device }
    , m_geometry{ other.m_geometry }
    , m_verBuf{ std::move(other.m_verBuf) }
    , m_idxBuf{ std::move(other.m_idxBuf) } {
  other.m_geometry = nullptr;
}


GameObject::~GameObject() {
  rtcReleaseGeometry(m_geometry);
}

GameObject& GameObject::operator=(GameObject&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  m_geometry = other.m_geometry;
  other.m_geometry = nullptr;
  m_verBuf = std::move(other.m_verBuf);
  m_idxBuf = std::move(other.m_idxBuf);
  return *this;
}

GameObject& GameObject::operator=(const GameObject& other) {
  if (this == &other) {
    return *this;
  }
  m_verBuf = other.m_verBuf;
  m_idxBuf = other.m_idxBuf;
  return *this;
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

  m_verBuf.resize(count[obj::Marker::V]);
  m_idxBuf.resize(count[obj::Marker::F]);

  if (!m_verBuf.size() || !m_idxBuf.size()) {
    return false;
  }

  std::string marker;
  char dummy[INPUT_BUFFER_SIZE];
  int verBufIndex = 0, idxBufIndex = 0;
  do {
    if (ifs.peek() == '#') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      continue;
    }
    ifs >> marker;
    switch (obj::ToMarker(marker)) {
      case obj::Marker::V: {
        std::string recv;
        ifs >> recv;
        m_verBuf[verBufIndex].x = std::stof(recv);
        ifs >> recv;
        m_verBuf[verBufIndex].y = std::stof(recv);
        ifs >> recv;
        m_verBuf[verBufIndex].z = std::stof(recv);
        ++verBufIndex;
      } break;

      case obj::Marker::F: {
        std::string recv;
        ifs >> recv;
        m_idxBuf[idxBufIndex].v0 = std::stoi(recv) - 1;
        ifs >> recv;
        m_idxBuf[idxBufIndex].v1 = std::stoi(recv) - 1;
        ifs >> recv;
        m_idxBuf[idxBufIndex].v2 = std::stoi(recv) - 1;
        ++idxBufIndex;
      } break;

      case obj::Marker::MTLLIB: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;

      case obj::Marker::USEMTL: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;

      case obj::Marker::G: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;

      case obj::Marker::VN: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;

      case obj::Marker::VT: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;

      default: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1�s��΂�
      } break;
    }
  } while (!ifs.eof());

  rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      &m_verBuf[0],
      0,
      sizeof(Vertex3f),
      count[obj::Marker::V]);


  rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT3,
      &m_idxBuf[0],
      0,
      sizeof(PolygonIndices),
      count[obj::Marker::F]);

  rtcCommitGeometry(m_geometry);
  return true;
}

uint32_t GameObject::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_geometry);
}

} // namespace cges