#pragma once

#include "RandomNumber.hpp"

#include <array>
#include <random>
#include <type_traits>
#include <vector>

template<typename T>
class RandomRealNumber : public RandomNumber<T>
{
  std::uniform_real_distribution<T> m_range;

public:
  RandomRealNumber(T min, T max);
  virtual T scalar() override;
  virtual std::vector<T> vector(std::size_t dimension) override;
  template<std::size_t S>
  std::array<T, S> vector();
};

template<typename T>
RandomRealNumber<T>::RandomRealNumber(T min, T max)
  : RandomNumber<T>()
  , m_range{min, max}
{
  // The type must be RealType
  constexpr bool isFloat = std::is_same_v<float, T>;
  constexpr bool isDouble = std::is_same_v<double, T>;
  constexpr bool isLongDouble = std::is_same_v<long double, T>;

  static_assert(isFloat | isDouble | isLongDouble);
}

template<typename T>
T
RandomRealNumber<T>::scalar()
{
  return m_range(this->m_engine);
}

template<typename T>
std::vector<T>
RandomRealNumber<T>::vector(std::size_t dimension)
{
  std::vector<T> vec;

  vec.reserve(dimension);
  for (std::size_t i = 0; i < dimension; ++i) {
    vec.push_back(m_range(this->m_engine));
  }

  return vec;
}

template<typename T>
template<std::size_t S>
std::array<T, S>
RandomRealNumber<T>::vector()
{
  std::array<T, S> vec;
  for (auto i = 0; i < S; ++i) {
    vec[i] = m_range(this->m_engine);
  }
  return vec;
}
