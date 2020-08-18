#pragma once

#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "node.hpp"
#include "random_real_number.hpp"

class Lattice
{
  int m_dimen;
  int m_maxIter;
  int m_remainingIter;
  float m_timeConst;
  float m_currRate;
  float m_initialRate;
  float m_neighborhoodRadius;
  RandomRealNumber<float> m_random;
  std::array<float, 3> m_center;
  std::vector<Node> m_neurons;

public:
  Lattice(int size, int iterations, float rate);
  bool input(std::vector<float> in);
  int dimension() const;
  int maxIterations() const;
  int currentIteration() const;
  float currentRate() const;
  float initialRate() const;
  float neighborhoodRadius() const;
  template<std::size_t S>
  bool input(std::array<float, S> in);
  std::vector<Node> const& neurons() const;
  std::vector<unsigned short> lineIndices() const;
  std::vector<unsigned short> triangleIndices() const;
};

template<std::size_t S>
inline bool
Lattice::input(std::array<float, S> in)
{
  return input(std::vector<float>(in.begin(), in.end()));
}
