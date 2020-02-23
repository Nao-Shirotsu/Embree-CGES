#pragma once

#include "renderer_base.hpp"

namespace cges::renderer {

class PhoneShader : public Base {
private:
  void ParallelDraw(const Camera& camera,
                    RenderBuffer& renderTarget,
                    const Scene& scene,
                    const int loopMin,
                    const int loopMax,
                    const glm::vec3& initialPos,
                    const glm::vec3& screenVerticalVec,
                    const glm::vec3& screenHorizontalVec) const override;
};

}// namespace cges::renderer