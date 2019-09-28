#pragma once

#include <string>
#include <cstdlib> // std::exit
#include <array>

namespace {
constexpr int INPUT_BUFFER_SIZE = 512;
}

namespace cges::obj {

// .objファイルの各行頭にあるマーカー
enum class Marker {
  V = 0,
  F,
  VT,
  VN,
  G,
  MTLLIB,
  USEMTL,
};

// ↑の6要素が1つのobjに何個含まれるか保存する構造体
class NumElements {
public:
  NumElements()
      : m_count{0, 0, 0, 0, 0, 0} {}

  inline size_t& operator[](const Marker idx) {
    return m_count[static_cast<int>(idx)];
  }

private:
  std::array<size_t, 6> m_count;
};

[[nodiscard]] inline Marker ToMarker(const std::string& marker) {
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
  std::exit(1);
}

inline bool CountCategories(std::ifstream& ifs, obj::NumElements& count) {
  char dummy[INPUT_BUFFER_SIZE];
  std::string markerStr;
  while (!ifs.eof()) {
    if (ifs.peek() == '#') {
      ifs.getline(dummy, INPUT_BUFFER_SIZE);
      continue;
    }
    ifs >> markerStr;
    ifs.getline(dummy, INPUT_BUFFER_SIZE);
    ++count[obj::ToMarker(markerStr)];
  }
  ifs.clear();
  ifs.seekg(std::ios::beg);
  return bool(ifs);
}

} // namespace cges::obj