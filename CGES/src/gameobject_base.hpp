#pragma once

#include "color.hpp"
#include "material_base.hpp"

#include <embree3/rtcore_geometry.h>
#include <embree3/rtcore_scene.h>
#include <glm/vec3.hpp>

namespace cges::gameobject{

// シーンに配置される任意の3Dモデルのクラス
class Base{
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

  virtual RTCGeometryType GetGeomType() const = 0;

  // ワールド座標位置を取得 (メンバ変数のアラインメントの都合で純粋仮想)
  virtual glm::vec3 GetPosWorld() const = 0;

  virtual glm::vec3 SampleSurfacePoint() const noexcept = 0;

  // このオブジェクトに向かってきたRayがどの方向に反射するか求める(マテリアルによって多態)
  glm::vec3 ComputeReflectedDir(const glm::vec3& faceNormal, const glm::vec3& incomingRayDir) const;

  // レンダリング方程式中のradianceにかける係数(インポータンスサンプリングを考慮した式変形後の形)
  glm::vec3 IntegrandFactor(const glm::vec3& surfacePoint,
                      const glm::vec3& outgoingDir,
                      const glm::vec3& incomingDir,
                      const glm::vec3& normal,
                      const glm::vec3& albedo) const;

  // Embree管理下のシーンにこのオブジェクトを登録
  void AttachTo(const RTCScene scene, const unsigned int geomID);

  bool IsEmitting() const noexcept;

  ColorRGBA GetEmission() const noexcept;

protected:
  const RTCDevice m_rtcDevice;
  RTCGeometry m_rtcGeometry;
  const material::Base& m_material;
  const ColorRGBA m_emissionColor;
  bool m_isEmitting;
};

} // namespace cges::gameobject