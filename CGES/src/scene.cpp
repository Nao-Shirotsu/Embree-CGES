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
  (*(m_objects.end() - 1))->AttachTo(m_rtcScene);
  sceneChanged = true;
}

const RTCScene Scene::GetRTCScene() const {
  return m_rtcScene;
}

const glm::vec3& Scene::GetDirLightForward() const {
  return m_dirLight.dir;
}

ColorRGBA Scene::GetObjectColor(const size_t idx, const float u, const float v) const{
  return m_objects[idx]->GetColorByUV(u, v);
}


} //namespace cges