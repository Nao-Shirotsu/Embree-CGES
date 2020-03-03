#pragma once

#include <vector>
#include <memory>

#include <glm/vec3.hpp>
#include <embree3/rtcore.h>

#include "gameobject_base.hpp"
#include "light_source.hpp"

namespace cges {

// ÉVÅ[Éì
class Scene {
public:
  Scene(const RTCDevice rtcDevice);
  ~Scene() noexcept;

  void Update();
  void Add(std::unique_ptr<gameobject::Base> object);
  const RTCScene GetRTCScene() const;
  const glm::vec3& GetDirLightForward() const;
  RTCGeometryType GetGeomType(const unsigned int objID) const;
  ColorRGBA GetGeomColor(const unsigned int objID, const float u, const float v) const;

private:
  Scene(const Scene& other) = delete;
  Scene& operator=(const Scene& other) = delete;

  RTCScene m_rtcScene;
  std::vector<std::unique_ptr<gameobject::Base>> m_objects;
  std::vector<DirectionalLight> m_lightSrcs;
  DirectionalLight m_dirLight;
  bool m_sceneChanged;
};

} // namespace