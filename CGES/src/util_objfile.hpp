#pragma once

#include "bufferelements.hpp"

#include <array>
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>

namespace cges::obj {

// filePathの.objファイルから引数のバッファにロード 返り値は成功 or NOT
bool LoadFromFile(const char* const filePath, const glm::vec3& posWorld, std::vector<glm::vec3>& vertexBuf, std::vector<PolygonIndex>& indexBuf, std::vector<glm::vec3>& vertexNormalBuf);

} // namespace cges::obj