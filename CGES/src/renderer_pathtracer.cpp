#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <algorithm> // std::clamp
#include <glm/glm.hpp>
#include <limits> // float minimum
#include <stack>

//#include <iostream>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();
constexpr float ROULETTE_HIT_RATE = 0.25f;
constexpr float PI = 3.14159265358979323846f;
constexpr unsigned int TRACE_INITIAL_ID = 1;
constexpr size_t TRACE_LIMIT = 64;
constexpr size_t NUM_SAMPLING = 32;

bool HasEmission(const cges::Scene& scene, const unsigned int geomID) {
  const auto& gameObj = scene.GetGeomRef(geomID);
  return gameObj.GetEmission().r > 0 && gameObj.GetEmission().g > 0 && gameObj.GetEmission().b > 0;
}

}

namespace cges::renderer {

void PathTracer::ParallelDraw(const Camera& camera,
                              RenderBuffer& renderTarget,
                              const Scene& scene,
                              const size_t loopMin,
                              const size_t loopMax,
                              const glm::vec3& initialPos,
                              const glm::vec3& screenVerticalVec,
                              const glm::vec3& screenHorizontalVec) const {
  const auto cameraPos = camera.posWorld + camera.GetPosLocal();
  RandomGenerator randomGen;
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGL�̕`���left-bottom-up�Ȃ̂ŋt�ɂ���
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      glm::vec3 rayDir = pixelPos - cameraPos;
      glm::vec3 rayOrg = cameraPos;
      RTCRayHit rayhit;
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      // primal ray ���΂�
      InitRayHit(rayhit, rayOrg, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      // primal ray�����ɂ��Փ˂��Ȃ�������w�i�F��`�悵�Ď��̃s�N�Z����
      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // primal ray���Փ˂����I�u�W�F�N�g�������̂��������A���̌��̐F��`�悵�Ď��̃s�N�Z����
      if (HasEmission(scene, rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx] = scene.GetGeomRef(rayhit.hit.geomID).GetEmission();
        continue;
      }

      // primal ray�������̈ȊO�ɏՓ˂������A�����_�����O�������̐ϕ������ċA�I�Ɍv�Z����
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f }; // MC�ϕ��̑��a��(���ƂŃT���v�����O�񐔂Ŋ���)
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        // �����_�����O�������̍���, RGB��[0.0f, 255.0f]�͈̔͂Œl��ێ�
        // 1�̌o�H���^�ԕ��ˋP�x��ێ�����
        glm::vec3 radiance = {0.0f, 0.0f, 0.0f};

        // �����_�Ɋւ���[BRDF��cos���̐�]�̒l��ۑ�����X�^�b�N
        std::stack<float> brdfCosStack; // �����炠����x�̑傫���̃o�b�t�@�p�ӂ��������X�^�b�N��葬������

        // ���ˋP�x�֐��]���̍ċA�񐔌��x�����V�����ŊǗ�
        RussianRoulette roulette(randomGen, TRACE_LIMIT, ROULETTE_HIT_RATE);

        // �����ɓ��B����܂Ńp�X�g��(��̌W�����X�^�b�N���Ă���)
        while (!roulette.Spin()) {
          // ���C��1�{��΂�
          InitRayHit(rayhit, rayOrg, rayDir);
          rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

          // ���ɂ��Փ˂��Ȃ������牽��������1�̌o�H�̃g���[�X�I��
          if (!WasIntersected(rayhit.hit.geomID)) {
            break;
          }

          // �����̂ɏՓ˂����炻��emission�𑫂���1�̌o�H�̃g���[�X�I��
          if (HasEmission(scene, rayhit.hit.geomID)) {
            {
              const auto emission = scene.GetGeomRef(rayhit.hit.geomID).GetEmission();
              radiance = { static_cast<float>(emission.r),
                           static_cast<float>(emission.g),
                           static_cast<float>(emission.b) };
            }
            break;
          }

          // �񔭌��̂ɏՓ˂�����ray�𔽎˂�����
          // �Ō�Ɍ�����������
          const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);

          // ���̖ʖ@��
          const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

          // ����Ray�̔��ˈʒu/���������߁A���[�v�̎��̎��̂��߂ɍX�V
          rayOrg = rayOrg + rayhit.ray.tfar * rayDir; 
          rayDir = intersectedObj.ComputeReflectedDir(faceNormal, rayDir);

          // BRDF*cos����ۑ�
          brdfCosStack.push(glm::dot(faceNormal, rayDir));
        }

        // ��̃p�X�g���[�X�Ō��ǔ����̂܂ł��ǂ蒅���Ȃ������ꍇ�AMC�ϕ��̎��̎��̕]����
        if (!WasIntersected(rayhit.hit.geomID)) {
          continue;
        }

        // �����܂Ńp�X�g���[�X���������Ă���Ȃ�A�����ɒH�蒅���͂�

        // �Ō�ɏՓ˂����I�u�W�F�N�g(�K�������ɂȂ�͂�)
        const auto& lastIntesectedObj = scene.GetGeomRef(rayhit.hit.geomID);

        // 1�̌o�H�̕��ˋP�x�̌v�Z
        // �~�ς���BRDF*cos�����Ō�ɏՓ˂���������emission�ɂ����Ă���
        while (!brdfCosStack.empty()) {
          radiance *= brdfCosStack.top();
          brdfCosStack.pop();
        }

        // MC�ϕ��̑��a���ɑ���(��ŃT���v�����O�񐔂Ŋ���)
        cumulativeRadiance += radiance;
      }

      cumulativeRadiance = cumulativeRadiance * 2.0f * PI / static_cast<float>(NUM_SAMPLING);
      cumulativeRadiance.r = std::clamp(cumulativeRadiance.r, 0.0f, 255.0f);
      cumulativeRadiance.g = std::clamp(cumulativeRadiance.g, 0.0f, 255.0f);
      cumulativeRadiance.b = std::clamp(cumulativeRadiance.b, 0.0f, 255.0f);
      renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(cumulativeRadiance.r);
      renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(cumulativeRadiance.g);
      renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(cumulativeRadiance.b);
      //std::cout << "(" << xIdx << ", " << yIdx << ") : {"
      //          << static_cast<int>(renderTarget[yIdx][xIdx].r) << ", "
      //          << static_cast<int>(renderTarget[yIdx][xIdx].g) << ", "
      //          << static_cast<int>(renderTarget[yIdx][xIdx].b) << "}"
      //          << std::endl;
      //int a = 5;
    }
  }
}

} // namespace cges::renderer