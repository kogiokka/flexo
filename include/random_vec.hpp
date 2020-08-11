#pragma once

#include <array>
#include <cstddef>
#include <random>

template<std::size_t D>
class RandomVec
{
  std::random_device m_rd;
  std::mt19937_64 m_engine;
  std::uniform_real_distribution<float> m_range;

public:
  RandomVec();
  std::array<float, D> vec();
};

template<size_t D>
RandomVec<D>::RandomVec()
  : m_rd{}
  , m_engine{m_rd()}
  , m_range{0.0f, 1.0f}
{
}

template<size_t D>
std::array<float, D>
RandomVec<D>::vec()
{
  std::array<float, D> vec;
  for (auto i = 0; i < D; ++i) {
    vec[i] = m_range(m_engine);
  }

  return vec;
}
