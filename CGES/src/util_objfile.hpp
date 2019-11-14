#pragma once

#include "bufferelements.hpp"

#include <array>
#include <fstream>
#include <vector>

namespace cges::obj {

// filePath��.obj�t�@�C����������̃o�b�t�@�Ƀ��[�h �Ԃ�l�͐��� or NOT
bool LoadFromFile(const char* const filePath, std::vector<Vector3f>& vertexBuf, std::vector<PolygonIndex>& indexBuf, std::vector<Vector3f>& vertexNormalBuf);

} // namespace cges::obj