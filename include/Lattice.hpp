#pragma once

#include "Node.hpp"
#include "RandomRealNumber.hpp"

#include <array>
#include <vector>

class Lattice
{
  int dimen_;
  int iterCap_;
  int iterRemained_;
  float timeConstant_;
  float rateCurrent_;
  float rateInitial_;
  float neighborhoodRadius_;
  RandomRealNumber<float> randomDevice_;
  std::vector<Node> neurons_;

public:
  Lattice(int size, int iterations, float rate);
  bool input(std::vector<float> in);
  int dimension() const;
  int iterationCap() const;
  int currentIteration() const;
  float currentRate() const;
  float initialRate() const;
  float neighborhoodRadius() const;
  template<std::size_t S>
  bool input(std::array<float, S> in);
  std::vector<Node> const& neurons() const;
  std::vector<unsigned int> edgeIndices() const;
  std::vector<unsigned int> faceIndices() const;
};

template<std::size_t S>
inline bool
Lattice::input(std::array<float, S> in)
{
  return input(std::vector<float>(in.begin(), in.end()));
}
