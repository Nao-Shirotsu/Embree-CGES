#include "randomgenerator.hpp"

namespace cges {

RandomGenerator::RandomGenerator()
    : mt(std::random_device()())
    , distr(0.0f, 1.0f) {}

float RandomGenerator::operator()() {
  return distr(mt);
}

}