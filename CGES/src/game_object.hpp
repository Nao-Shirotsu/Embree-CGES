#pragma once

#include <cstdint>
#include <vector>
#include <glm/vec3.hpp>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace cges {

struct Vertex3f {
  float x, y, z;
};

struct VertexNormal3f {
  float nx, ny, nz;
};

struct PolygonIndices {
  unsigned int v0, v1, v2;
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

  // 頂点属性(法線など)を持っているかどうか
  bool HasAttribute();

  // Embree管理下のシーンにこのオブジェクトを登録
  uint32_t AttachTo(const RTCScene scene);

private:
  //glm::vec3 origin; // モデル座標系原点のワールド位置
  const RTCDevice m_device;
  RTCGeometry m_rtcGeometry;
  std::vector<Vertex3f> m_verBuf;
  std::vector<PolygonIndices> m_idxBuf;
  std::vector<VertexNormal3f> m_verNormalBuf;
};

} // namespace cges