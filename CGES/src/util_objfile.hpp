#pragma once

#include <string>
#include <cstdlib> // std::exit

namespace cges::obj {

// .objファイルの各行頭にあるマーカー
enum class Marker {
  V,
  F,
  VT,
  VN,
  G,
  MTLLIB,
  USEMTL,
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

} // namespace cges::obj