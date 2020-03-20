#pragma once

#include "color.hpp"
#include "material_base.hpp"

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>
#include <glm/vec3.hpp>

namespace cges::gameobject{

// シーンに配置される任意の3Dモデルのクラス
class Base {
public:
  Base(const RTCDevice device, 
       const RTCGeometryType geomType, 
       const ColorRGBA emissionColor, 
       const material::Base& material);

  virtual ~Base() noexcept;

  // 非必須属性(Embreeのバッファのslotに格納する値)を持っているかどうか
  virtual bool HasAttribute() = 0;

  // オブジェクトのテクスチャ座標から色取得
  virtual ColorRGBA GetColor(const float u, const float v) const = 0;

  ColorRGBA GetEmission() const;

  virtual RTCGeometryType GetGeomType() const = 0;

  // ワールド座標位置を取得 (メンバ変数のアラインメントの都合で純粋仮想)
  virtual glm::vec3 GetPosWorld() const = 0;

  // このオブジェクトに向かってきたRayがどの方向に反射するか求める(マテリアルによって多態)
  glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const;

  // RGBごとのBRDFを評価する(マテリアルによって多態)
  glm::vec3 BRDF(const glm::vec3& surfacePoint,
                 const glm::vec3& outgoingDir,
                 const glm::vec3& incomingDir,
                 const glm::vec3& normal,
                 const ColorRGBA albedo) const;

  // Embree管理下のシーンにこのオブジェクトを登録
  void AttachTo(const RTCScene scene, const unsigned int geomID);

protected:
  const RTCDevice m_rtcDevice;
  RTCGeometry m_rtcGeometry;
  const ColorRGBA m_emissionColor;
  const material::Base& m_material;
};

} // namespace cges::gameobject