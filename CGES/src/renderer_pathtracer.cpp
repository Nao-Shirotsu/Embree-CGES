#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <algorithm> // std::clamp
#include <glm/glm.hpp>
#include <limits> // float minimum
#include <stack>

namespace {

constexpr float EPSILON = 1.0e-6f;
constexpr float MIN = std::numeric_limits<float>::min();

constexpr size_t TRACE_LIMIT = 64;
constexpr float ROULETTE_HIT_RATE = 0.25f;

bool HasEmission(const cges::Scene& scene, const unsigned int geomID) {
  return scene.GetGeomEmission(geomID).r > 0 && scene.GetGeomEmission(geomID).g > 0 && scene.GetGeomEmission(geomID).b > 0;
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
  RussianRoulette roulette(TRACE_LIMIT, ROULETTE_HIT_RATE);
  for (size_t y = loopMin; y < loopMax; ++y) {
    const float yRate = y / static_cast<float>(renderTarget.GetHeight());
    const float bgColorIntensity = 255 * (1.0f - yRate);
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGLの描画はleft-bottom-upなので逆にする
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      const glm::vec3 primalRayDir = pixelPos - cameraPos;
      RTCRayHit rayhit;
      InitRayHit(rayhit, cameraPos, primalRayDir);

      // 通った経路の交差点を順に保存するスタック
      std::stack<glm::vec3> intersectionStack;

      // 光源に到達するまでパストレ
      while (!roulette.Spin() && WasIntersected(rayhit.hit.geomID) && !HasEmission(scene, rayhit.hit.geomID)) {
        glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
      }

      // primal rayがどこにも衝突していない or ロシルレがトレース回数限度に達した
      if (intersectionStack.size() == 0 || intersectionStack.size() > TRACE_LIMIT) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // パストレの1つ経路が無事光源に到達した時、サンプリング1回分の放射輝度の計算開始
      glm::vec3 cumulativeRadiance; // レンダリング方程式の左辺
      {
        const auto emission = scene.GetGeomEmission(rayhit.hit.geomID);
        cumulativeRadiance = { static_cast<float>(emission.r),
                               static_cast<float>(emission.g),
                               static_cast<float>(emission.b) };
      }
      glm::vec3 incomingRayOrg = intersectionStack.top();
      intersectionStack.pop();
      glm::vec3 currentPoint;
      while (intersectionStack.size() != 0) {
        currentPoint = intersectionStack.top();
        const auto incomingRayDir = currentPoint - incomingRayOrg;
        intersectionStack.pop();
        const auto outgoingRayDir = intersectionStack.top() - currentPoint;
        //cumulativeRadiance = 

        incomingRayOrg = currentPoint;
      }
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