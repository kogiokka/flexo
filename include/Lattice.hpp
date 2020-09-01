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
  bool Input(std::vector<float> in);
  int Dimension() const;
  int IterationCap() const;
  int CurrentIteration() const;
  float CurrentRate() const;
  float InitialRate() const;
  float NeighborhoodRadius() const;
  template<std::size_t S>
  bool Input(std::array<float, S> in);
  std::vector<Node> const& Neurons() const;
  std::vector<unsigned int> EdgeIndices() const;
  std::vector<unsigned int> FaceIndices() const;
};

template<std::size_t S>
inline bool
Lattice::Input(std::array<float, S> in)
{
  return Input(std::vector<float>(in.begin(), in.end()));
}
