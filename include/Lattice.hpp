#pragma once

#include "Node.hpp"
#include "RandomRealNumber.hpp"

#include <array>
#include <vector>

class Lattice
{
  int width_;
  int height_;
  float lenDiag_;
  int iterCap_;
  int iterRemained_;
  float timeConstant_;
  float rateCurrent_;
  float rateInitial_;
  float neighborhoodRadius_;
  RandomRealNumber<float> RNG_;
  std::vector<Node> neurons_;

public:
  Lattice(int width, int height, int iterations, float initRate);
  bool Input(std::vector<float> in);
  int Width() const;
  int Height() const;
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
