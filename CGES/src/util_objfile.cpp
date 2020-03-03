#include "util_objfile.hpp"

#include <string>

namespace {

constexpr int INPUT_BUFFER_SIZE = 512;

// .objファイルの各行頭にあるマーカー
enum class Marker {
  V = 0,
  F,
  VT,
  VN,
  G,
  MTLLIB,
  USEMTL,
  SKIP, // コメント及び不明なマーカ
};

// ↑の6要素が1つのobjに何個含まれるか保存する構造体
class NumElements {
public:
  NumElements()
      : m_count{ 0, 0, 0, 0, 0, 0 } {}

  size_t& operator[](const Marker idx) {
    return m_count[static_cast<int>(idx)];
  }

private:
  std::array<size_t, 7> m_count;
};

void ReadObjFLine(const std::string& fStatement, size_t& vertex, size_t& tex, size_t& normal) noexcept {
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

Marker ToMarker(const std::string& marker) {
  if (marker == "v") {
    return Marker::V;
  }
  if (marker == "f") {
    return Marker::F;
  }
  if (marker == "vt") {
    return Marker::VT;
  }
  if (marker == "vn") {
    return Marker::VN;
  }
  if (marker == "g") {
    return Marker::G;
  }
  if (marker == "mtllib") {
    return Marker::MTLLIB;
  }
  if (marker == "usemtl") {
    return Marker::USEMTL;
  }
  return Marker::SKIP;
}

bool LoadFileProperties(std::ifstream& ifs, NumElements& count, bool& fTriple) {
  using namespace cges;
  using namespace cges::obj;
  char dummy[INPUT_BUFFER_SIZE];
  bool fTripleWritten = false;
  std::string markerStr;
  while (!ifs.eof()) {
    const auto next = ifs.peek();
    if (next == '#' || next == ' ' || next == '\n') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE);
      continue;
    }
    ifs >> markerStr;
    const auto marker = ToMarker(markerStr);
    if (marker == Marker::F && !fTripleWritten) {
      ifs >> markerStr;
      const auto pos = markerStr.find_first_of("/");
      fTriple = (pos == std::string::npos ? false : true);
      fTripleWritten = true;
    }
    ifs.getline(dummy, INPUT_BUFFER_SIZE);
    if (marker == Marker::SKIP) {
      continue;
    }
    ++count[marker];
  }
  ifs.clear();
  ifs.seekg(0, std::ios::beg);
  return bool(ifs);
}

}

namespace cges::obj {

bool LoadFromFile(const char* const filePath, const glm::vec3& posWorld, std::vector<glm::vec3>& vertexBuf, std::vector<PolygonIndex>& indexBuf, std::vector<glm::vec3>& vertexNormalBuf) {
  auto ifs = std::ifstream(filePath);
  if (!ifs) {
    return false;
  }
  NumElements count;
  bool fTriple = false;
  if (!LoadFileProperties(ifs, count, fTriple)) {
    return false;
  }

  vertexBuf.resize(count[Marker::V]);
  indexBuf.resize(count[Marker::F]);
  vertexNormalBuf.resize(count[Marker::V]);

  std::vector<glm::vec3> vnBuf{ 0 };

  if (fTriple) {
    vnBuf.resize(count[Marker::VN]);
  }

  if (!vertexBuf.size() || !indexBuf.size()) {
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
    switch (ToMarker(marker)) {
      case Marker::V: {
        ifs >> vertexBuf[verBufIndex].x;
        ifs >> vertexBuf[verBufIndex].y;
        ifs >> vertexBuf[verBufIndex].z;
        vertexBuf[verBufIndex].x += posWorld.x;
        vertexBuf[verBufIndex].y += posWorld.y;
        vertexBuf[verBufIndex].z += posWorld.z;
        ++verBufIndex;
      } break;

      case Marker::F: {
        std::string recv;
        if (!fTriple) {
          std::string recv;
          ifs >> recv;
          indexBuf[idxBufIndex].v0 = std::stoi(recv) - 1;
          ifs >> recv;
          indexBuf[idxBufIndex].v1 = std::stoi(recv) - 1;
          ifs >> recv;
          indexBuf[idxBufIndex].v2 = std::stoi(recv) - 1;
        }
        else {
          size_t verIdx, texIdx, normIdx;
          ifs >> recv;
          ReadObjFLine(recv, verIdx, texIdx, normIdx);
          indexBuf[idxBufIndex].v0 = verIdx - 1;
          vertexNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadObjFLine(recv, verIdx, texIdx, normIdx);
          indexBuf[idxBufIndex].v1 = verIdx - 1;
          vertexNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];

          ifs >> recv;
          ReadObjFLine(recv, verIdx, texIdx, normIdx);
          indexBuf[idxBufIndex].v2 = verIdx - 1;
          vertexNormalBuf[verIdx - 1] = vnBuf[normIdx - 1];
        }
        ++idxBufIndex;
      } break;

      case Marker::MTLLIB: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case Marker::USEMTL: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case Marker::G: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      case Marker::VN: {
        ifs >> vnBuf[vnBufIndex].x;
        ifs >> vnBuf[vnBufIndex].y;
        ifs >> vnBuf[vnBufIndex].z;
        ++vnBufIndex;
      } break;

      case Marker::VT: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;

      default: {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
      } break;
    }
  } while (!ifs.eof());

  return true;
}

}