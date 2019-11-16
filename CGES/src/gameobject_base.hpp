#pragma once

#include "color.hpp"

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>

namespace cges::gameobject{

// シーンに配置される任意の3Dモデルのクラス
class Base {
public:
  Base(const RTCDevice device, const RTCGeometryType geomType);

  virtual ~Base() noexcept;

  // 非必須属性(Embreeのバッファのslotに格納する値)を持っているかどうか
  virtual bool HasAttribute() = 0;

  // Embree管理下のシーンにこのオブジェクトを登録
  unsigned int AttachTo(const RTCScene scene);

  // uv座標からピクセル色を返す
  virtual ColorRGBA GetColorByUV(const float x, const float y) = 0;

protected:
  //glm::vec3 origin; // モデル座標系原点のワールド位置
  const RTCDevice m_rtcDevice;
  RTCGeometry m_rtcGeometry;
};

} // namespace cges::gameobject