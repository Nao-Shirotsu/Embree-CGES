#pragma once

#include <vector>
#include <memory>
#include <type_traits>

#include <glm/vec3.hpp>
#include <embree3/rtcore.h>

#include "gameobject_base.hpp"
#include "directionallight.hpp"

namespace cges {

// ÉVÅ[Éì
class Scene {
public:
  Scene(const RTCDevice rtcDevice);
  ~Scene() noexcept;

  void Update();
  void Add(std::unique_ptr<gameobject::Base> object);
  const RTCScene GetRTCScene() const;
  const gameobject::Base& GetGeomRef(const unsigned int objID) const;
  const std::vector<unsigned int>& GetLightIndices() const;

private:
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;

  RTCScene m_rtcScene;
  std::vector<std::unique_ptr<gameobject::Base>> m_objects;
  std::vector<unsigned int> m_lightIdx;
  bool m_sceneChanged;
};

} // namespace