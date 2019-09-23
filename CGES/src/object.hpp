#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec3.hpp>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace shi {

// 描画されるモデルのデータを格納/操作するクラス
class Object {
public:
  // とりあえず.obj(vとfのみ)からの読み込みだけ実装する
  inline Object(const RTCDevice device, const RTCScene scene)
      : geometry{ nullptr }
      , vertices{ nullptr }
      , indices{ nullptr } {
    geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  }

  inline ~Object() {
    rtcReleaseGeometry(geometry);
  }

  // 引数:
  // 返値:
  // 説明:
  inline void LoadObjFile(const char* const objFileName) {
    // TODO: ちゃんと実装する
    vertices = static_cast<float*>(rtcSetNewGeometryBuffer(geometry,
                                                           RTC_BUFFER_TYPE_VERTEX,
                                                           0,
                                                           RTC_FORMAT_FLOAT3,
                                                           3 * sizeof(float),
                                                           3));
    indices = static_cast<uint32_t*>(rtcSetNewGeometryBuffer(geometry,
                                                             RTC_BUFFER_TYPE_INDEX,
                                                             0,
                                                             RTC_FORMAT_UINT3,
                                                             3 * sizeof(uint32_t),
                                                             1));
    if (vertices && indices) {
      vertices[0] = 0.f;
      vertices[1] = 0.f;
      vertices[2] = 0.f;
      vertices[3] = 1.f;
      vertices[4] = 0.f;
      vertices[5] = 0.f;
      vertices[6] = 0.f;
      vertices[7] = 1.f;
      vertices[8] = 0.f;
      indices[0] = 0;
      indices[1] = 1;
      indices[2] = 2;
    }
    rtcCommitGeometry(geometry);
  }

  // 引数:
  // 返値:
  // 説明:
  inline uint32_t AttachTo(const RTCScene scene) {
    return rtcAttachGeometry(scene, geometry);
  }

private:
  //glm::vec3 origin; // モデル座標系原点
  RTCGeometry geometry;
  float* vertices;   // すべての頂点(VertexBuffer)
  uint32_t* indices; // すべてのポリゴンの頂点番号(Index Buffer)
};

} // namespace shi