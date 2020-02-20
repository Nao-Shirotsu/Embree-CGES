#include "camera.hpp"
#include "renderbuffer.hpp"
#include "renderer.hpp"
#include "glengine.hpp"
#include "scene.hpp"
#include "gameobject_sphere.hpp"
#include "gameobject_polygonalmesh.hpp"

#include <embree3/rtcore.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto embreeDevice = rtcNewDevice(nullptr);
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0, 0, 0 }, 40.0f);
  auto graphicsEngine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
  auto scene = cges::Scene(embreeDevice);
  auto renderer = cges::Renderer();

  //scene.Add(cges::MakePolygonalMesh(embreeDevice, "bin/goat_filled.obj", {64, 64, 255}));
  scene.Add(cges::MakeSphere(embreeDevice, 1.25f, "bin/aizu_library.jpg"));

  while (!graphicsEngine.ShouldTerminate()) {
    graphicsEngine.Update(camera);
    scene.Update();
    renderer.Draw(camera, renderTarget, scene);
    graphicsEngine.Draw(renderTarget);
  }

  rtcReleaseDevice(embreeDevice);
}

// TODO:
// - gameobject::PolygonModel::LoadObjFile�ŁA�}�[�J�["v"��"f"�ȊO���Ή�����悤�Ɏ�������
// - Renderer::Update�̃��C�g������������������
// - gl::Engine::Update��fps����Ɛ�������������
// - DXR�f���̂悤�Ƀm�C�Y���t���[�����Ƃɏ��X�Ɍ��炷�`��ɂ���
// - opengl�̃o�[�W�����w����Œ�łȂ�����
// - gameobject::Base���O�p�`�ȊO�ɂ��Ή�������(������grid�����ł悳����)
// - SSE4, AVX�Ή��𒲂ׂ�intersect1~intersect4�Ȃǂ�؂�ւ���

// effect������
// - ������
// - GI
// - reflection
// - �e�N�X�`���}�b�s���O
// - �g�D�[���V�F�[�f�B���O
// - �O�[���[�V�F�[�f�B���O
// - �u���[
// - �o���v�}�b�s���O