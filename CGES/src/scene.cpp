#include "scene.hpp"

#include <cmath>

namespace cges {

Scene::Scene(const RTCDevice rtcDevice)
    : m_rtcScene{ rtcNewScene(rtcDevice) }
    , m_objects( 0 )
    //, m_lightIdx( 0 )
    , m_sceneChanged{false} {}

Scene::~Scene() noexcept {
  rtcReleaseScene(m_rtcScene);
}

void Scene::Update() {
  if (!m_sceneChanged) {
    return;
  }
  rtcCommitScene(m_rtcScene);
  m_sceneChanged = false;
}

void Scene::Add(std::unique_ptr<gameobject::Base> object) {
  m_objects.push_back(std::move(object));
  (*(m_objects.end() - 1))->AttachTo(m_rtcScene, static_cast<unsigned int>(m_objects.size()) - 1);
  m_sceneChanged = true;
}

const RTCScene Scene::GetRTCScene() const {
  return m_rtcScene;
}

const gameobject::Base& Scene::GetGeomRef(const unsigned int objID) const {
  return *(m_objects[objID]);
}

} //namespace cges