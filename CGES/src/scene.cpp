#include "scene.hpp"

#include <cmath>

namespace cges {

Scene::Scene(const RTCDevice rtcDevice)
    : m_rtcScene{ rtcNewScene(rtcDevice) }
    , m_objects{ 0 }
    , m_lightSrcs{ 0 }
    , sceneChanged{false} {
  const float divRoot3 = 1.0f / std::sqrt(3.0f);
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

void Scene::Add(GameObject&& object) {
  m_objects.push_back(std::move(object));
  (m_objects.end() - 1)->AttachTo(m_rtcScene);
  sceneChanged = true;
}

void Scene::Emplace(const RTCDevice device, const char* const filePath) {
  m_objects.emplace_back(device, filePath);
  (m_objects.end() - 1)->AttachTo(m_rtcScene);
  sceneChanged = true;
}

const RTCScene Scene::GetScenePtr() const {
  return m_rtcScene;
}


} //namespace cges
