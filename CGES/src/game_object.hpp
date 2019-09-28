#pragma once

#include <fstream>
#include <string>
#include <cstdio>

#include <glm/vec3.hpp>

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

#include "util_objfile.hpp"

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
  inline GameObject(const RTCDevice device)
      : m_geometry{ nullptr } {
    m_geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  }

  inline ~GameObject() {
    rtcReleaseGeometry(m_geometry);
  }

  // ロード成功でtrue, 失敗でfalseを返す
  inline bool LoadObjFile(const char* const objFileName) {
    auto ifs = std::ifstream(objFileName);
    if (!ifs) {
      return false;
    }
    obj::NumElements count;
    if (!obj::CountCategories(ifs, count)) {
      return false;
    }

    auto verBuf = reinterpret_cast<PolygonVertices*>(
        rtcSetNewGeometryBuffer(
            m_geometry,
            RTC_BUFFER_TYPE_VERTEX,
            0,
            RTC_FORMAT_FLOAT3,
            sizeof(PolygonVertices),
            count[obj::Marker::V]));

    auto idxBuf = reinterpret_cast<PolygonIndices*>(
        rtcSetNewGeometryBuffer(
            m_geometry,
            RTC_BUFFER_TYPE_INDEX,
            0,
            RTC_FORMAT_UINT3,
            sizeof(PolygonIndices),
            count[obj::Marker::F]));

    if (!verBuf || !idxBuf) {
      return false;
    }

    std::string marker;
    char dummy[INPUT_BUFFER_SIZE];

    for (int verBufIndex = 0, idxBufIndex = 0; !ifs.eof();) {
      if (ifs.peek() == '#') {
        ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
        continue;
      }
      ifs >> marker;
      switch (obj::ToMarker(marker)) {
        case obj::Marker::V: {
          ifs >> verBuf[verBufIndex].x;
          ifs >> verBuf[verBufIndex].y;
          ifs >> verBuf[verBufIndex].z;
          ++verBufIndex;
        } break;

        case obj::Marker::F: {
          ifs >> idxBuf[idxBufIndex].v0;
          ifs >> idxBuf[idxBufIndex].v1;
          ifs >> idxBuf[idxBufIndex].v2;
          ++idxBufIndex;
        } break;

        default: {
          ifs.getline(dummy, INPUT_BUFFER_SIZE); // 1行飛ばす
        } break;
      }
    }

    rtcCommitGeometry(m_geometry);
    return true;
  }


  inline uint32_t AttachTo(const RTCScene scene) {
    return rtcAttachGeometry(scene, m_geometry);
  }

private:
  //glm::vec3 origin; // モデル座標系原点
  RTCGeometry m_geometry;
};

} // namespace cges