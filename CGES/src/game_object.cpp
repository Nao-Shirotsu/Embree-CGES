#include "game_object.hpp"

#include "constants.hpp"
#include "util_objfile.hpp"

#include <array>
#include <cstdio>
#include <fstream>
#include <string>

namespace cges {

// .objの f 節の値を読み取る   "1/2/3"なら渡した引数に1, 2, 3を書き込む"
void ReadFTriple(const std::string& fStatement, size_t& vertex, size_t& tex, size_t& normal) noexcept {
  vertex = std::stoi(fStatement);
  const auto firstSlashPos = fStatement.find_first_of("/");
  const auto secondSlashPos = fStatement.find_last_of("/");
  if (fStatement[firstSlashPos + 1] != '/') {
    tex = std::stoi(fStatement.substr(firstSlashPos + 1, secondSlashPos - firstSlashPos - 1));
  }
  normal = std::stoi(fStatement.substr(secondSlashPos + 1, fStatement.length() - secondSlashPos - 1));
  // str: 100/200/300
  // idx: 0123456789
}

GameObject::GameObject(const RTCDevice device)
    : m_device{ device }
    , m_rtcGeometry{ rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE) }
    , m_verBuf{ 0 }
    , m_idxBuf{ 0 }
    , m_verNormalBuf{ 0 } {
}

GameObject::GameObject(const RTCDevice device, const char* const filePath)
    : GameObject(device) {
  bool success = LoadObjFile(filePath);
  assert(success);
}

GameObject::GameObject(const GameObject& other)
    : m_device{ other.m_device }
    , m_rtcGeometry{ rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE) }
    , m_verBuf{ other.m_verBuf }
    , m_idxBuf{ other.m_idxBuf }
    , m_verNormalBuf{ other.m_verNormalBuf } {
}

GameObject::GameObject(GameObject&& other) noexcept
    : m_device{ other.m_device }
    , m_rtcGeometry{ other.m_rtcGeometry }
    , m_verBuf{ std::move(other.m_verBuf) }
    , m_idxBuf{ std::move(other.m_idxBuf) }
    , m_verNormalBuf{ std::move(other.m_verNormalBuf) } {
  other.m_rtcGeometry = nullptr;
}


GameObject::~GameObject() {
  rtcReleaseGeometry(m_rtcGeometry);
}

GameObject& GameObject::operator=(GameObject&& other) noexcept {
  if (this == &other) {
    return *this;
  }
  m_rtcGeometry = other.m_rtcGeometry;
  other.m_rtcGeometry = nullptr;
  m_verBuf = std::move(other.m_verBuf);
  m_idxBuf = std::move(other.m_idxBuf);
  m_verNormalBuf = std::move(other.m_verNormalBuf);
  return *this;
}

GameObject& GameObject::operator=(const GameObject& other) {
  if (this == &other) {
    return *this;
  }
  m_verBuf = other.m_verBuf;
  m_idxBuf = other.m_idxBuf;
  m_verNormalBuf = other.m_verNormalBuf;
  return *this;
}

bool GameObject::LoadObjFile(const char* const objFileName) {
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
  m_verNormalBuf.resize(count[obj::Marker::V]);

  std::vector<VertexNormal3f> vnBuf{ 0 };
  
  if (fTriple) {
    vnBuf.resize(count[obj::Marker::VN]);
  }

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
        ifs >> m_verBuf[verBufIndex].x;
        ifs >> m_verBuf[verBufIndex].y;
        ifs >> m_verBuf[verBufIndex].z;
        ++verBufIndex;
      } break;

      case obj::Marker::F: {
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
          m_verNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadFTriple(recv, verIdx, texIdx, normIdx);
          m_idxBuf[idxBufIndex].v1 = verIdx - 1;
          m_verNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadFTriple(recv, verIdx, texIdx, normIdx);
          m_idxBuf[idxBufIndex].v2 = verIdx - 1;
          m_verNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];
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
        ifs >> vnBuf[vnBufIndex].nx;
        ifs >> vnBuf[vnBufIndex].ny;
        ifs >> vnBuf[vnBufIndex].nz;
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

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX,
      0,
      RTC_FORMAT_FLOAT3,
      m_verBuf.data(),
      0,
      sizeof(Vertex3f),
      m_verBuf.size());

  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_INDEX,
      0,
      RTC_FORMAT_UINT3,
      m_idxBuf.data(),
      0,
      sizeof(PolygonIndices),
      m_idxBuf.size());

  rtcSetGeometryVertexAttributeCount(m_rtcGeometry, 1);
  rtcSetSharedGeometryBuffer(
      m_rtcGeometry,
      RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
      0,
      RTC_FORMAT_FLOAT3,
      m_verNormalBuf.data(),
      0,
      sizeof(VertexNormal3f),
      m_verNormalBuf.size());

  rtcCommitGeometry(m_rtcGeometry);

  return true;
}

bool GameObject::HasAttribute() {
  return !(m_verNormalBuf.empty());
}

uint32_t GameObject::AttachTo(const RTCScene scene) {
  return rtcAttachGeometry(scene, m_rtcGeometry);
}

} // namespace cges