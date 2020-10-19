#include "pm_photon.hpp"

namespace cges::pm {

float& Photon3D::operator[](const size_t idx) {
  return pos[idx];
}

float& Photon3D::X() {
  return pos[0];
}

float& Photon3D::Y() {
  return pos[1];
}

float& Photon3D::Z() {
  return pos[2];
}

}