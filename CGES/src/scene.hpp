#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <embree3/rtcore.h>

#include "game_object.hpp"
#include "light_source.hpp"

namespace cges {

// シーン
class Scene {
public:
  Scene(const RTCDevice rtcDevice);
  ~Scene() noexcept;

  void Update();
  void Add(GameObject&& object); // GameObjectのコピー禁止なので const& ではなく &&
  const RTCScene GetScenePtr() const;

private:
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;

  RTCScene m_rtcScene;
  std::vector<GameObject> m_objects;
  std::vector<LightSource> m_lightSrcs;
  LightSource m_dirLight;
  bool sceneChanged;
};

} // namespace