#pragma once

#include <array>
#include <cstddef>
#include <random>
#include <vector>

template<typename T>
class RandomNumber
{
protected:
  std::random_device m_device;
  std::mt19937_64 m_engine;

public:
  RandomNumber();
  virtual ~RandomNumber();
  virtual T scalar() = 0;
  virtual std::vector<T> vector(std::size_t dimension) = 0;
  template<std::size_t S>
  std::array<T, S> vector();
};

template<typename T>
RandomNumber<T>::RandomNumber()
  : m_device{}
  , m_engine(m_device())
{
}

template<typename T>
RandomNumber<T>::~RandomNumber()
{
}
