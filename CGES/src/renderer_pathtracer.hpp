#pragma once

#include "renderer_base.hpp"

namespace cges::renderer {

class PathTracer : public Base {
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

} // namespace cges::renderer