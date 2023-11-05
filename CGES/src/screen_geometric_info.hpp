#pragma once

#include "Camera.hpp"

#include <glm/vec3.hpp>

namespace cges {

// RenderBufferの幾何学的パラメータ構造体
// ワールド座標に浮いているスクリーンバッファの位置とか
struct ScreenGeometricInfo {
  ScreenGeometricInfo(const Camera& camera, const float aspectRatio) noexcept;

  const glm::vec3& GetCenterPos() const noexcept;
  const glm::vec3& GetUpleftPos() const noexcept;
  const glm::vec3& GetHorizontVec() const noexcept;
  const glm::vec3& GetVerticalVec() const noexcept;

private:
  glm::vec3 m_centerPos;
  glm::vec3 m_horizontVec;
  glm::vec3 m_verticalVec;
  glm::vec3 m_upleftPos;
};

}// namespace cges