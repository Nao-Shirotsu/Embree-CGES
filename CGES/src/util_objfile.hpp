#pragma once

#include "bufferelements.hpp"

#include <array>
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>

namespace cges::obj {

// filePath��.obj�t�@�C����������̃o�b�t�@�Ƀ��[�h �Ԃ�l�͐��� or NOT
bool LoadFromFile(const char* const filePath, const glm::vec3& posWorld, std::vector<Vector3f>& vertexBuf, std::vector<PolygonIndex>& indexBuf, std::vector<Vector3f>& vertexNormalBuf);

} // namespace cges::obj