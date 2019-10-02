#pragma once

#include <array>
#include <fstream>
#include <string>

namespace cges::obj {

// .obj�t�@�C���̊e�s���ɂ���}�[�J�[
enum class Marker {
  V = 0,
  F,
  VT,
  VN,
  G,
  MTLLIB,
  USEMTL,
  SKIP, // �R�����g�y�ѕs���ȃ}�[�J
};

// ����6�v�f��1��obj�ɉ��܂܂�邩�ۑ�����\����
class NumElements {
public:
  NumElements();
  size_t& operator[](const Marker idx);

private:
  std::array<size_t, 7> m_count;
};

[[nodiscard]] Marker ToMarker(const std::string& marker);
bool CountCategories(std::ifstream& ifs, obj::NumElements& count);

} // namespace cges::obj