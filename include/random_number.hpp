#pragma once

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
  virtual T get() = 0;
  virtual std::vector<T> vector(std::size_t dimension) = 0;
};

template<typename T>
RandomNumber<T>::RandomNumber()
  : m_device{}
  , m_engine(m_device())
{
}
