#pragma once

#include <random>

namespace cges {

class RandomGenerator {
public:
  RandomGenerator();
  float operator()();

private:
  std::mt19937_64 mt;
  std::uniform_real_distribution<float> distr;
};

}