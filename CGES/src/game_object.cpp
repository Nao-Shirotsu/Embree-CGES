#include "game_object.hpp"

#include "constants.hpp"
#include "util_objfile.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <array>

#ifdef _DEBUG
#include <iostream>
#endif

namespace cges {

// .objの f 節の値を読み取る   "1/2/3"なら渡した引数に1, 2, 3を書き込む"
void ReadFTriple(const std::string& fStatement, size_t& vertex, size_t& tex, size_t& normal) noexcept{
  vertex = std::stoi(fStatement);
  const auto firstSlashPos = fStatement.find_first_of("/");
  const auto secondSlashPos = fStatement.find_last_of("/");
  tex = std::stoi(fStatement.substr(firstSlashPos + 1, secondSlashPos - firstSlashPos - 1));
  normal = std::stoi(fStatement.substr(secondSlashPos + 1, fStatement.length() - secondSlashPos - 1));
    // str: 100/200/300
    // idx: 0123456789
}

GameObject::GameObject(const RTCDevice device)
    : m_device{ device }
    , m_geometry{ rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE) }
    , m_verBuf{ 0 }
    , m_idxBuf{ 0 } {
}

GameObject::GameObject(const RTCDevice device, const char* const filePath)
    : GameObject(device) {
  bool success = LoadObjFile(filePath, device);
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

bool GameObject::LoadObjFile(const char* const objFileName, const RTCDevice device) {
  auto ifs = std::ifstream(objFileName);
  if (!ifs) {
    return false;
  }
  obj::NumElements count;
  bool fTriple = false;
  if (!obj::LoadFileProperties(ifs, count, fTriple)) {
    return false;
  }

  m_verBuf.resize(count[obj::Marker::V]);
  m_idxBuf.resize(count[obj::Marker::F]);
  std::vector<glm::vec3> vnBuf{ count[obj::Marker::VN] };

  if (!m_verBuf.size() || !m_idxBuf.size()) {
    return false;
  }

  std::string marker;
  char dummy[INPUT_BUFFER_SIZE];
  int verBufIndex = 0, idxBufIndex = 0, vnBufIndex = 0;
  do {
    if (ifs.peek() == '#') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      continue;
    }
    ifs >> marker;
    switch (obj::ToMarker(marker)) {
      case obj::Marker::V: {
        ifs >> m_verBuf[verBufIndex].pos.x;
        ifs >> m_verBuf[verBufIndex].pos.y;
        ifs >> m_verBuf[verBufIndex].pos.z;
        ++verBufIndex;
      } break;

      case obj::Marker::F: {
        if (idxBufIndex == m_idxBuf.size()) {
          int a = 4;
        }
        std::string recv;
        if (!fTriple) {
          std::string recv;
          ifs >> recv;
          m_idxBuf[idxBufIndex].v0 = std::stoi(recv) - 1;
          ifs >> recv;
          m_idxBuf[idxBufIndex].v1 = std::stoi(recv) - 1;
          ifs >> recv;
          m_idxBuf[idxBufIndex].v2 = std::stoi(recv) - 1;
        }
        else {
          size_t verIdx, texIdx, normIdx;
          ifs >> recv;
          ReadFTriple(recv, verIdx, texIdx, normIdx);
          m_idxBuf[idxBufIndex].v0 = verIdx - 1;
          m_verBuf[verIdx - 1].normal = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadFTriple(recv, verIdx, texIdx, normIdx);
          m_idxBuf[idxBufIndex].v1 = verIdx - 1;
          m_verBuf[verIdx - 1].normal = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadFTriple(recv, verIdx, texIdx, normIdx);
          m_idxBuf[idxBufIndex].v2 = verIdx - 1;
          m_verBuf[verIdx - 1].normal = vnBuf[normIdx - 1];
        }
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
        ifs >> vnBuf[vnBufIndex].x;
        ifs >> vnBuf[vnBufIndex].y;
        ifs >> vnBuf[vnBufIndex].z;
        ++vnBufIndex;
      } break;

      case obj::Marker::VT: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      default: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;
    }
  } while (!ifs.eof());

  auto err = rtcGetDeviceError(device);

  rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
      0,
      RTC_FORMAT_FLOAT8,
      m_verBuf.data(),
      0,
      sizeof(Vertex3f),
      m_verBuf.size());

  err = rtcGetDeviceError(device);

  rtcSetSharedGeometryBuffer(
      m_geometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT4,
      m_idxBuf.data(),
      0,
      sizeof(PolygonIndices),
      m_idxBuf.size());

  err = rtcGetDeviceError(device);

  #ifdef _DEBUG
  std::cout << "Vertex Buffer : stride=" << sizeof(Vertex3f) << "   start=" << std::hex << &m_verBuf[0] << std::dec << std::endl;
  std::cout << "Index Buffer : stride=" << sizeof(PolygonIndices) << "   start=" << std::hex << &m_idxBuf[0] << std::dec << std::endl;
  #endif

  rtcCommitGeometry(m_geometry);

  return true;
}

uint32_t GameObject::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_geometry);
}

} // namespace cges