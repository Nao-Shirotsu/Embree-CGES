#include "util_objfile.hpp"

#include "constants.hpp"

#include <cstdlib>

namespace cges::obj {

NumElements::NumElements()
    : m_count{ 0, 0, 0, 0, 0, 0 } {}

size_t& NumElements::operator[](const Marker idx) {
  return m_count[static_cast<int>(idx)];
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
  std::exit(1);
}

bool CountCategories(std::ifstream& ifs, obj::NumElements& count) {
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

}