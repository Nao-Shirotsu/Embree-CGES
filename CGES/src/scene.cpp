#include "scene.hpp"

#include <cmath>

namespace cges {

Scene::Scene(const RTCDevice rtcDevice)
    : m_rtcScene{ rtcNewScene(rtcDevice) }
    , m_objects( 0 )
    , m_lightSrcs( 0 )
    , sceneChanged{false} {
  const float divRoot3 = 1.0f / std::sqrtf(3.0f);
  m_dirLight.dir = { -divRoot3, -divRoot3, -divRoot3 };
  m_dirLight.emitColor = { 128, 128, 128, 255 };
}

Scene::~Scene() noexcept {
  rtcReleaseScene(m_rtcScene);
}

void Scene::Update() {
  if (!sceneChanged) {
    return;
  }
  rtcCommitScene(m_rtcScene);
  sceneChanged = false;
}

void Scene::Add(std::unique_ptr<gameobject::Base> object) {
  m_objects.push_back(std::move(object));
  (*(m_objects.end() - 1))->AttachTo(m_rtcScene, m_objects.size() - 1);
  sceneChanged = true;
}

const RTCScene Scene::GetRTCScene() const {
  return m_rtcScene;
}

const glm::vec3& Scene::GetDirLightForward() const {
  return m_dirLight.dir;
}

RTCGeometryType Scene::GetGeomType(const unsigned int objID) const{
  int n = objID;
  return m_objects[objID]->GetGeomType();
}

ColorRGBA Scene::GetGeomColor(const unsigned int objID, const float u, const float v) const{
  int n = objID;
  return m_objects[objID]->GetColor(u, v);
}

} //namespace cges