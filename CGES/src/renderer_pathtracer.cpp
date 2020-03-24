#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <algorithm> // std::clamp
#include <glm/glm.hpp>
#include <limits> // float minimum
#include <stack>

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
  RussianRoulette roulette(randomGen, TRACE_LIMIT, ROULETTE_HIT_RATE);
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGL�̕`���left-bottom-up�Ȃ̂ŋt�ɂ���
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f };
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        glm::vec3 rayDir = pixelPos - cameraPos;
        glm::vec3 rayOrg = cameraPos;
        RTCRayHit rayhit;
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        // �����_�����O�������̍��� [0.0f, 1.0f]�Œl������
        glm::vec3 radiance;

        // �����_�Ɋւ���[BRDF��cos���̐�]�̒l��ۑ�����X�^�b�N
        std::stack<float> brdfCosStack; // �����炠����x�̑傫���̃o�b�t�@�p�ӂ��������X�^�b�N��葬������

        // �����ɓ��B����܂Ńp�X�g��(��̌W�����X�^�b�N���Ă���)
        while (!roulette.Spin()) {
          // ���C��1�{��΂�
          InitRayHit(rayhit, rayOrg, rayDir);
          rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);
          if (!WasIntersected(rayhit.hit.geomID) || HasEmission(scene, rayhit.hit.geomID)) {
            break;
          }

          // �Ō�Ɍ�����������
          const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);

          // ���̖ʖ@��
          const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

          // ����Ray���g���[�X���锭�ˈʒu/���������߁A���[�v�̎��̎��̂��߂ɍX�V
          rayOrg = rayOrg + rayhit.ray.tfar * rayDir; 
          rayDir = intersectedObj.ComputeReflectedDir(faceNormal, rayDir);

          // BRDF*cos����ۑ�
          brdfCosStack.push(glm::dot(faceNormal, rayDir));
        }

        // primal ray���ǂ��ɂ��Փ˂��Ă��Ȃ� or ���V�������g���[�X�񐔌��x�ɒB����
        if (brdfCosStack.empty() || brdfCosStack.size() > TRACE_LIMIT) {
          radiance = { 0.0f, 0.0f, 0.0f };
          continue;
        }

        // �Ō�ɏՓ˂����I�u�W�F�N�g(�K�������ɂȂ�͂�)
        const auto& lastIntesectedObj = scene.GetGeomRef(rayhit.hit.geomID);

        // �p�X�g����1�o�H�����������ɓ��B�������A�T���v�����O1�񕪂̕��ˋP�x�̌v�Z�J�n
        {
          const auto emission = lastIntesectedObj.GetEmission();
          radiance = { static_cast<float>(emission.r),
                                 static_cast<float>(emission.g),
                                 static_cast<float>(emission.b) };
        }

        while (!brdfCosStack.empty()) {
          radiance *= brdfCosStack.top();
          brdfCosStack.pop();
        }

        cumulativeRadiance += radiance;
      }

      if (cumulativeRadiance == glm::vec3(0.0f, 0.0f, 0.0f)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      cumulativeRadiance = cumulativeRadiance * 2.0f * PI / static_cast<float>(NUM_SAMPLING);
      cumulativeRadiance.r = std::clamp(cumulativeRadiance.r, 0.0f, 1.0f);
      cumulativeRadiance.g = std::clamp(cumulativeRadiance.g, 0.0f, 1.0f);
      cumulativeRadiance.b = std::clamp(cumulativeRadiance.b, 0.0f, 1.0f);
      renderTarget[yIdx][xIdx].r = static_cast<uint8_t>((cumulativeRadiance.r * 255.0f));
      renderTarget[yIdx][xIdx].g = static_cast<uint8_t>((cumulativeRadiance.g * 255.0f));
      renderTarget[yIdx][xIdx].b = static_cast<uint8_t>((cumulativeRadiance.b * 255.0f));
    }
  }
}

} // namespace cges::renderer