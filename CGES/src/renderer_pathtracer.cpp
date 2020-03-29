#include "renderer_pathtracer.hpp"
#include "util_renderer.hpp"

#include <algorithm> // std::clamp
#include <glm/glm.hpp>
#include <limits> // float minimum
#include <stack>

#include <cassert>
#include <iostream>

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

glm::vec3 ComputeRadiance(RTCIntersectContext& context, 
                          const glm::vec3& rayOrg, 
                          const glm::vec3& incomingDir, 
                          cges::RussianRoulette& roulette,
                          const cges::Scene& scene) {
  using namespace cges;

  RTCRayHit rayhit;
  InitRayHit(rayhit, rayOrg, incomingDir);
  rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

  if (!WasIntersected(rayhit.hit.geomID)) {
    return { 0.0f, 0.0f, 0.0f };
  }

  const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);
  const glm::vec3 surfaceEmission = { static_cast<float>(intersectedObj.GetEmission().r),
                                      static_cast<float>(intersectedObj.GetEmission().g),
                                      static_cast<float>(intersectedObj.GetEmission().b) };

  if (roulette.Spin()) {
    return surfaceEmission;
  }

  const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));
  const glm::vec3 surfacePos = rayOrg + rayhit.ray.tfar * incomingDir;
  const glm::vec3 outgoingDir = intersectedObj.ComputeReflectedDir(faceNormal, incomingDir);
  const glm::vec3 incomingRadiance = ComputeRadiance(context, surfacePos, outgoingDir, roulette, scene);
  const glm::vec3 brdfValue = intersectedObj.BRDF(surfacePos, outgoingDir, incomingDir, faceNormal, intersectedObj.GetColor(0.0f, 0.0f));

  return surfaceEmission + incomingRadiance * brdfValue * glm::dot(faceNormal, -incomingDir) * 2.0f * PI;
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

      // primal rayが何かに衝突した時、レンダリング方程式の右辺を再帰的に計算する
      glm::vec3 cumulativeRadiance = { 0.0f, 0.0f, 0.0f }; // MC積分の総和項(あとでサンプリング回数で割る)
      for (auto sampling = 0u; sampling < NUM_SAMPLING; ++sampling) {
        RussianRoulette roulette(randomGen, TRACE_LIMIT, ROULETTE_HIT_RATE);
        //// 交差点に関する[BRDF*cosΘ/pdf]の値を保存するスタック
        //std::stack<float> factorStack; // 元からある程度の大きさのバッファ用意した方がスタックより速いかも

        //// 交差点の発光量(RGB)を保存するスタック
        //std::stack<glm::vec3> emissionStack;

        //// 放射輝度関数評価の再帰回数限度をロシルレで管理
        //RussianRoulette roulette(randomGen, TRACE_LIMIT, ROULETTE_HIT_RATE);

        //// 反射回数限度までパストレ(上の係数をスタックしていく)
        //while (true) {
        //  if (roulette.Spin()) {
        //    emissionStack.push({ 0.0f, 0.0f, 0.0f });
        //    break;
        //  }

        //  // レイを1本飛ばす
        //  InitRayHit(rayhit, rayOrg, rayDir);
        //  rtcIntersect1(scene.GetRTCScene(), &context, &rayhit);

        //  // 何にも衝突しなかったら1つの経路のトレース終了
        //  if (!WasIntersected(rayhit.hit.geomID)) {
        //    emissionStack.push({ 0.0f, 0.0f, 0.0f });
        //    break;
        //  }

        //  // 最後に交差した物体
        //  const auto& intersectedObj = scene.GetGeomRef(rayhit.hit.geomID);

        //  // 衝突物体のemissionをスタックにpush
        //  emissionStack.push({ static_cast<float>(intersectedObj.GetEmission().r),
        //                        static_cast<float>(intersectedObj.GetEmission().g),
        //                        static_cast<float>(intersectedObj.GetEmission().b) });

        //  // ↑の面法線
        //  const glm::vec3 faceNormal = glm::normalize(glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

        //  const glm::vec3 surfacePos = rayOrg + rayhit.ray.tfar * rayDir;
        //  const glm::vec3 outgoingDir = intersectedObj.ComputeReflectedDir(faceNormal, rayDir);
        //  //std::cout << "normal : (" << faceNormal.x << ", " << faceNormal.y << ", " << faceNormal.z << ")\n";
        //  //std::cout << "outDir : (" << outgoingDir.x << ", " << outgoingDir.y << ", " << outgoingDir.z << ")\n"<< std::endl;

        //  const float brdfVal = intersectedObj.BRDF(surfacePos, -rayDir, outgoingDir, faceNormal, intersectedObj.GetColor(0.0f, 0.0f)).r;

        //  // 次のRayの発射位置/方向を求め、ループの次の周のために更新
        //  rayOrg = rayOrg + rayhit.ray.tfar * rayDir;
        //  rayDir = outgoingDir;

        //  // BRDF*cosΘを保存   
        //  factorStack.push(brdfVal * glm::dot(faceNormal, -rayDir));
        //  /////////////////////////////////////////////
        //  /*             ↑      ↑      ↑
        //  * diffuseしか考えない形になってるのであとで修正しろ
        //  * ちょっと上のbrdfValも
        //  *
        //  *////////////////////////////////////////////
        //}

        //// 1つの経路が運ぶ放射輝度
        //glm::vec3 radiance = emissionStack.top();
        //emissionStack.pop();

        ////std::cout << "E : " << emissionStack.size() << "\nF : " <<  factorStack.size() << std::endl;;

        //while (!factorStack.empty()) {
        //  radiance *= factorStack.top();
        //  factorStack.pop();
        //  radiance += emissionStack.top();
        //  emissionStack.pop();
        //}
        //radiance *= 2.0f * PI;

        //// MC積分の総和項に足す(後でサンプリング回数で割る)
        //cumulativeRadiance += radiance;
        cumulativeRadiance += ComputeRadiance(context, rayOrg, rayDir, roulette, scene);
      }

      cumulativeRadiance = cumulativeRadiance / static_cast<float>(NUM_SAMPLING);

      // オーバーフロー対策で一応clampする
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