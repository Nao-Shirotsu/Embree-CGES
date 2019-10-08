#pragma once

#include <cstdint>
#include <vector>
#include <glm/vec3.hpp>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace cges {

struct Vertex3f {
  glm::vec3 pos;
  float padding1;
  glm::vec3 normal;
  float padding2;
  //uint8_t padding[16 - (sizeof(pos) + sizeof(normal)) % 16];
};

struct PolygonIndices {
  unsigned int v0, v1, v2;
  unsigned int padding[1];
  //uint8_t padding[16 - (sizeof(v0) * 3 % 16)];
};

// シーンに配置される任意の3Dモデルのクラス
class GameObject {
public:
  GameObject(const RTCDevice device); // 空のオブジェクト
  GameObject(const RTCDevice device, const char* const filePath); // 3Dモデル読込
  GameObject(const GameObject& other);

  GameObject(GameObject&& other) noexcept;

  ~GameObject() noexcept;

  GameObject& operator=(GameObject&& other) noexcept;
  GameObject& operator=(const GameObject& other);

  // ロード成功でtrue, 失敗でfalseを返す
  bool LoadObjFile(const char* const objFileName);
  uint32_t AttachTo(const RTCScene scene);

private:
  //glm::vec3 origin; // モデル座標系原点のワールド位置
  const RTCDevice m_device;
  RTCGeometry m_geometry;
  std::vector<Vertex3f> m_verBuf;
  std::vector<PolygonIndices> m_idxBuf;
};

} // namespace cges