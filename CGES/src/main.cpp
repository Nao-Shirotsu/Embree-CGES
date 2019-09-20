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
  /* �R�p�`�W�I���g�����쐬 */
  RTCGeometry mesh = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
  /*���_�̐ݒ�*/
  Vertex* vertices = ( Vertex* )rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 4);
  vertices[0].x = 10;
  vertices[0].y = 10;
  vertices[0].z = 0; // �_A�̍��W
  vertices[1].x = 10;
  vertices[1].y = -10;
  vertices[1].z = 0; // �_B�̍��W
  vertices[2].x = -10;
  vertices[2].y = 10;
  vertices[2].z = 0; // �_C�̍��W
  vertices[3].x = -10;
  vertices[3].y = -10;
  vertices[3].z = 0; // �_D�̍��W

  /*�ʂ̐ݒ�(�p��?)*/
  int tri = 0;
  Triangle* triangles = ( Triangle* )rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);
  triangles[tri].v0 = 0;
  triangles[tri].v1 = 1;
  triangles[tri].v2 = 3;
  tri++; // �O�p�`ABD
  triangles[tri].v0 = 1;
  triangles[tri].v1 = 2;
  triangles[tri].v2 = 3;
  tri++; // �O�p�`BCD

  unsigned int geomID = rtcAttachGeometry(scene, mesh);


  /*�W�I���g���̍폜*/
  rtcReleaseGeometry(mesh);

  /* �V�[���֓o�^ */
  rtcCommitGeometry(mesh);

  /*/*���C�����̏�����*/
  RTCIntersectContext context;
  rtcInitIntersectContext(&context);
  RTCHit rtchit;
  rtchit.geomID = RTC_INVALID_GEOMETRY_ID;

  /* ���C�𐶐����� */
  RTCRay rtcray;
  /* ���C�̎n�_ */
  rtcray.org_x = 0.0;  // x
  rtcray.org_y = 0.0;  // y
  rtcray.org_z = 20.0; // z
  /* ���C�̕��� */
  rtcray.dir_x = 0.0;  // x
  rtcray.dir_y = 0.0;  // y
  rtcray.dir_z = -1.0; // z
  /* �������肷��͈͂��w�� */
  rtcray.tnear = 0.0f; // �͈͂̎n�_
  rtcray.tfar = inf;   // �͈͂̏I�_�D���������ɂ͌����_�܂ł̋������i�[�����D
  rtcray.time = 0;

  /* �������� */
  RTCRayHit ray;
  rtcIntersect1(scene, &context, &ray);
  if (rtchit.geomID == RTC_INVALID_GEOMETRY_ID) {
    /* �����_��������Ȃ������ꍇ */
    std::cout << "Reject." << std::endl;
  }
  else {
    /* �����_�����������ꍇ */
    std::cout << "Intersect" << std::endl;
  }

  /* �V�[�����폜 */
  rtcReleaseScene(scene);
  /* �f�o�C�X���폜 */
  rtcReleaseDevice(device);

  std::cout << "aaa" << std::endl;
}