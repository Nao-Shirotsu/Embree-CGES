#pragma once

#include "bufferelements.hpp"

#include <array>
#include <fstream>
#include <vector>

namespace cges::obj {

// filePathの.objファイルから引数のバッファにロード 返り値は成功 or NOT
bool LoadFromFile(const char* const filePath, std::vector<Vector3f>& vertexBuf, std::vector<PolygonIndex>& indexBuf, std::vector<Vector3f>& vertexNormalBuf);

} // namespace cges::obj