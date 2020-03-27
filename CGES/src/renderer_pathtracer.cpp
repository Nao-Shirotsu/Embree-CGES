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
    const size_t yIdx = renderTarget.GetHeight() - y - 1; // openGLの描画はleft-bottom-upなので逆にする
    for (size_t xIdx = 0; xIdx < renderTarget.GetWidth(); ++xIdx) {
      const float xRate = xIdx / static_cast<float>(renderTarget.GetWidth());
      const glm::vec3 pixelPos = initialPos + (yRate * screenVerticalVec) + (xRate * screenHorizontalVec);
      glm::vec3 rayDir = pixelPos - cameraPos;
      glm::vec3 rayOrg = cameraPos;
      RTCRayHit rayhit;
      RTCIntersectContext context;
      rtcInitIntersectContext(&context);

      // primal ray を飛ばす
      InitRayHit(rayhit, rayOrg, rayDir);
      rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

      // primal rayが何にも衝突しなかったら背景色を描画して次のピクセルへ
      if (!WasIntersected(rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx].r = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].g = static_cast<uint8_t>(bgColorIntensity / 1.5f);
        renderTarget[yIdx][xIdx].b = static_cast<uint8_t>(bgColorIntensity);
        continue;
      }

      // primal rayが衝突したオブジェクトが発光体だった時、その光の色を描画して次のピクセルへ
      if (HasEmission(scene, rayhit.hit.geomID)) {
        renderTarget[yIdx][xIdx] = scene.GetGeomRef(rayhit.hit.geomID).GetEmission();
        continue;
      }

      // primal rayが発光体以外に衝突した時、レンダリング方程式の積分項を再帰的に計算する
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f }; // MC積分の総和項(あとでサンプリング回数で割る)
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        // レンダリング方程式の左辺, RGBを[0.0f, 255.0f]の範囲で値を保持
        // 1つの経路が運ぶ放射輝度を保持する
        glm::vec3 radiance = {0.0f, 0.0f, 0.0f};

        // 交差点に関する[BRDFとcosΘの積]の値を保存するスタック
        std::stack<float> brdfCosStack; // 元からある程度の大きさのバッファ用意した方がスタックより速いかも

        // 放射輝度関数評価の再帰回数限度をロシルレで管理
        RussianRoulette roulette(randomGen, TRACE_LIMIT, ROULETTE_HIT_RATE);

        // 光源に到達するまでパストレ(上の係数をスタックしていく)
        while (!roulette.Spin()) {
          // レイを1本飛ばす
          InitRayHit(rayhit, rayOrg, rayDir);
          rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

          // 何にも衝突しなかったら何も足さず1つの経路のトレース終了
          if (!WasIntersected(rayhit.hit.geomID)) {
            break;
          }

          // 発光体に衝突したらそのemissionを足して1つの経路のトレース終了
          if (HasEmission(scene, rayhit.hit.geomID)) {
            {
              const auto emission = scene.GetGeomRef(rayhit.hit.geomID).GetEmission();
              radiance = { static_cast<float>(emission.r),
                           static_cast<float>(emission.g),
                           static_cast<float>(emission.b) };
            }
            break;
          }

          // 非発光体に衝突したらrayを反射させる
          // 最後に交差した物体
          const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);

          // ↑の面法線
          const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

          // 次のRayの発射位置/方向を求め、ループの次の周のために更新
          rayOrg = rayOrg + rayhit.ray.tfar * rayDir; 
          rayDir = intersectedObj.ComputeReflectedDir(faceNormal, rayDir);

          // BRDF*cosΘを保存
          brdfCosStack.push(glm::dot(faceNormal, rayDir));
        }

        // 上のパストレースで結局発光体までたどり着けなかった場合、MC積分の次の周の評価へ
        if (!WasIntersected(rayhit.hit.geomID)) {
          continue;
        }

        // 光源までパストレースが成功しているなら、ここに辿り着くはず

        // 最後に衝突したオブジェクト(必ず光源になるはず)
        const auto& lastIntesectedObj = scene.GetGeomRef(rayhit.hit.geomID);

        // 1つの経路の放射輝度の計算
        // 蓄積したBRDF*cosΘを最後に衝突した光源のemissionにかけていく
        while (!brdfCosStack.empty()) {
          radiance *= brdfCosStack.top();
          brdfCosStack.pop();
        }

        // MC積分の総和項に足す(後でサンプリング回数で割る)
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