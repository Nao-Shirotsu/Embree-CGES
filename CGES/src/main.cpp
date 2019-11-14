#include "camera.hpp"
#include "renderbuffer.hpp"
#include "renderer.hpp"
#include "glengine.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 450;

int main() {
  auto renderTarget = cges::RenderBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
  auto camera = cges::Camera({ 0, 0, -1.125 }, 40.0f);
  auto renderer = cges::Renderer(camera, renderTarget);

  auto engine = cges::gl::Engine(WINDOW_WIDTH, WINDOW_HEIGHT, "Interactive Raytracer");
  while(!engine.ShouldTerminate()) {
    engine.Update(camera);
    renderer.Update();
    renderer.Draw();
    engine.Draw(renderTarget);
  }
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