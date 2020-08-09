#pragma once

#include <array>
#include <random>

class RandomVec3
{
  std::random_device m_rd;
  std::mt19937_64 m_engine;
  std::uniform_real_distribution<float> m_range;

public:
  RandomVec3();
  std::array<float, 3> color();
};
