#pragma once

#include <array>
#include <fstream>
#include <string>

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
  SKIP, // コメント及び不明なマーカ
};

// ↑の6要素が1つのobjに何個含まれるか保存する構造体
class NumElements {
public:
  NumElements();
  size_t& operator[](const Marker idx);

private:
  std::array<size_t, 7> m_count;
};

[[nodiscard]] Marker ToMarker(const std::string& marker);
bool LoadFileProperties(std::ifstream& ifs, obj::NumElements& count, bool fTriple);

} // namespace cges::obj