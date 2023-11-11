#pragma once

#include "renderer_base.hpp"

namespace cges::renderer {

class PhongShader : public Base {
public:
  PhongShader();

  void Update(const Camera& camera) override;

  ColorRGBf ComputeLightTransport(const Scene& scene,
                                  const glm::vec3& org,
                                  const glm::vec3& dir) const override;

private:
  void ParallelDraw(const Camera& camera,
                    RenderBuffer& renderTarget,
                    const Scene& scene,
                    const size_t loopMin,
                    const size_t loopMax,
                    const glm::vec3& initialPos,
                    const glm::vec3& screenVerticalVec,
                    const glm::vec3& screenHorizontalVec) const override;
};

}// namespace cges::renderer