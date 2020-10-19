#pragma once

#include <array>
#include <initializer_list>

namespace cges::pm {

// フォトンマッピングで使うフォトン構造体
struct Photon3D {
  float& operator[](const size_t idx);
  float& X();
  float& Y();
  float& Z();

  std::array<float, 3> pos;
};

}