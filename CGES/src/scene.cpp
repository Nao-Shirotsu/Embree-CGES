#include "scene.hpp"

#include <cmath>

namespace {

bool IsEmitting(cges::ColorRGBA emission) {
  return emission.r > 0 || emission.g > 0 || emission.b > 0;
}

}// anonymous namespace

namespace cges {

Scene::Scene(const RTCDevice rtcDevice)
    : m_rtcScene{ rtcNewScene(rtcDevice) }
    , m_objects( 0 )
    , m_lightIdx( 0 )
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
  const auto geomId = static_cast<unsigned int>(m_objects.size()) - 1;
  m_objects.back()->AttachTo(m_rtcScene, geomId);
  m_sceneChanged = true;

  if (IsEmitting(m_objects.back()->GetEmission())) {
    m_lightIdx.push_back(geomId);
  }
}

const RTCScene Scene::GetRTCScene() const {
  return m_rtcScene;
}

const gameobject::Base& Scene::GetGeomRef(const unsigned int objID) const {
  return *(m_objects[objID]);
}

const std::vector<unsigned int>& Scene::GetLightIndices() const {
  return m_lightIdx;
}

} //namespace cges