#include <iostream>
#include <cstdint>
#include <numeric>

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

constexpr int32_t inf = std::numeric_limits<int32_t>::max();

struct Triangle {
  int32_t v0;
  int32_t v1;
  int32_t v2;
};

struct Vertex {
  float x;
  float y;
  float z;
};

int main() {
  RTCDevice device = rtcNewDevice(nullptr);
  RTCScene scene = rtcNewScene(device);
  /* ３角形ジオメトリを作成 */
  RTCGeometry mesh = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  /*頂点の設定*/
  Vertex* vertices = ( Vertex* )rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 4);
  vertices[0].x = 10;
  vertices[0].y = 10;
  vertices[0].z = 0; // 点Aの座標
  vertices[1].x = 10;
  vertices[1].y = -10;
  vertices[1].z = 0; // 点Bの座標
  vertices[2].x = -10;
  vertices[2].y = 10;
  vertices[2].z = 0; // 点Cの座標
  vertices[3].x = -10;
  vertices[3].y = -10;
  vertices[3].z = 0; // 点Dの座標

  /*面の設定(用意?)*/
  int tri = 0;
  Triangle* triangles = ( Triangle* )rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);
  triangles[tri].v0 = 0;
  triangles[tri].v1 = 1;
  triangles[tri].v2 = 3;
  tri++; // 三角形ABD
  triangles[tri].v0 = 1;
  triangles[tri].v1 = 2;
  triangles[tri].v2 = 3;
  tri++; // 三角形BCD

  unsigned int geomID = rtcAttachGeometry(scene, mesh);


  /*ジオメトリの削除*/
  rtcReleaseGeometry(mesh);

  /* シーンへ登録 */
  rtcCommitGeometry(mesh);

  /*/*レイ交差の初期化*/
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCHit rtchit;
  rtchit.geomID = RTC_INVALID_GEOMETRY_ID;

  /* レイを生成する */
  RTCRay rtcray;
  /* レイの始点 */
  rtcray.org_x = 0.0;  // x
  rtcray.org_y = 0.0;  // y
  rtcray.org_z = 20.0; // z
  /* レイの方向 */
  rtcray.dir_x = 0.0;  // x
  rtcray.dir_y = 0.0;  // y
  rtcray.dir_z = -1.0; // z
  /* 交差判定する範囲を指定 */
  rtcray.tnear = 0.0f; // 範囲の始点
  rtcray.tfar = inf;   // 範囲の終点．交差判定後には交差点までの距離が格納される．
  rtcray.time = 0;

  /* 交差判定 */
  RTCRayHit ray;
  rtcIntersect1(scene, &context, &ray);
  if (rtchit.geomID == RTC_INVALID_GEOMETRY_ID) {
    /* 交差点が見つからなかった場合 */
    std::cout << "Reject." << std::endl;
  }
  else {
    /* 交差点が見つかった場合 */
    std::cout << "Intersect" << std::endl;
  }

  /* シーンを削除 */
  rtcReleaseScene(scene);
  /* デバイスを削除 */
  rtcReleaseDevice(device);

  std::cout << "aaa" << std::endl;
}