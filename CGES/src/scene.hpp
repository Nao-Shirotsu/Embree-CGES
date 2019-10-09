#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <embree3/rtcore.h>

#include "game_object.hpp"
#include "light_source.hpp"

namespace cges {

// ÉVÅ[Éì
class Scene {
public:
  Scene(const RTCDevice rtcDevice);
  ~Scene() noexcept;

  void Update();
  void Add(GameObject&& object);
  void Emplace(const RTCDevice device, const char* const filePath); // íºê⁄ç\íz
  const RTCScene GetRTCScene() const;

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