#pragma once

#include <cstdint>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace cges {

struct PolygonVertices {
  float x, y, z;
};

struct PolygonIndices {
  unsigned int v0, v1, v2;
};

// シーンに配置される任意の3Dモデルのクラス
class GameObject {
public:
  // とりあえず.obj(vとfのみ)からの読み込みだけ実装する
  GameObject(const RTCDevice device);
  ~GameObject();

  // ロード成功でtrue, 失敗でfalseを返す
  bool LoadObjFile(const char* const objFileName);
  uint32_t AttachTo(const RTCScene scene);

private:
  //glm::vec3 origin; // モデル座標系原点
  RTCGeometry m_geometry;
};

} // namespace cges