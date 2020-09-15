#include "Lattice.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>

Lattice::Lattice(int dimen, int iterations, float learningRate)
  : dimen_(dimen)
  , iterCap_(iterations)
  , iterRemained_(iterations)
  , rateCurrent_(learningRate)
  , rateInitial_(learningRate)
  , neighborhoodRadius_(dimen)
  , randomDevice_(-1.0f, 1.0f)
{
  for (int i = 0; i < dimen; ++i) {
    for (int j = 0; j < dimen; ++j) {
      neurons_.emplace_back(i, j, randomDevice_.vector(3));
    }
  }
  timeConstant_ = iterations / log(dimen);
}

bool
Lattice::Input(std::vector<float> in)
{
  if (iterRemained_ <= 0)
    return false;

  int const iterCurrent = (iterCap_ - iterRemained_);

  // Decay of neighborhood and learning rate
  neighborhoodRadius_ = dimen_ * exp(-iterCurrent / timeConstant_);
  rateCurrent_ = rateInitial_ * expf(-iterCurrent / iterRemained_);

  std::unique_ptr<Node> bmu;
  float distMin = std::numeric_limits<float>::max();

  // Find the Best Matching Unit
  for (Node& node : neurons_) {
    float sum = 0;
    for (int i = 0; i < node.dimension(); ++i) {
      float const diff = node[i] - in[i];
      sum += diff * diff;
    }
    if (distMin > sum) {
      bmu = std::make_unique<Node>(node);
      distMin = sum;
    }
  }

  for (Node& node : neurons_) {
    float const dx = bmu->X() - node.X();
    float const dy = bmu->Y() - node.Y();
    float const distToBmuSqr = dx * dx + dy * dy;
    float const radiusSqr = (neighborhoodRadius_ * neighborhoodRadius_);
    if (distToBmuSqr < radiusSqr) {
      float const influence = expf(-distToBmuSqr / (2.0f * radiusSqr)); // Why does (2.0f * radiusSqr) need parens?
      for (int i = 0; i < node.dimension(); ++i) {
        node[i] += rateCurrent_ * influence * (in[i] - node[i]);
      }
    }
  }

  --iterRemained_;

  return true;
}

int
Lattice::Dimension() const
{
  return dimen_;
}

int
Lattice::IterationCap() const
{
  return iterCap_;
}

int
Lattice::CurrentIteration() const
{
  return iterCap_ - iterRemained_;
}

float
Lattice::NeighborhoodRadius() const
{
  return neighborhoodRadius_;
}

std::vector<Node> const&
Lattice::Neurons() const
{
  return neurons_;
}

float
Lattice::InitialRate() const
{
  return rateInitial_;
}

float
Lattice::CurrentRate() const
{
  return rateCurrent_;
}

std::vector<unsigned int>
Lattice::EdgeIndices() const
{
  std::vector<unsigned int> indices;

  for (int i = 0; i < dimen_ - 1; ++i) {
    for (int j = 0; j < dimen_ - 1; ++j) {
      indices.push_back(i * dimen_ + j);
      indices.push_back(i * dimen_ + j + 1);
      indices.push_back(i * dimen_ + j + 1);
      indices.push_back((i + 1) * dimen_ + j + 1);
      indices.push_back((i + 1) * dimen_ + j + 1);
      indices.push_back((i + 1) * dimen_ + j);
      indices.push_back((i + 1) * dimen_ + j);
      indices.push_back(i * dimen_ + j);
    }
  }

  return indices;
}

std::vector<unsigned int>
Lattice::FaceIndices() const
{
  std::vector<unsigned int> indices;

  for (int i = 0; i < dimen_ - 1; ++i) {
    for (int j = 0; j < dimen_ - 1; ++j) {
      indices.push_back(i * dimen_ + j);
      indices.push_back((i + 1) * dimen_ + j);
      indices.push_back(i * dimen_ + j + 1);
      indices.push_back(i * dimen_ + j + 1);
      indices.push_back((i + 1) * dimen_ + j);
      indices.push_back((i + 1) * dimen_ + j + 1);
    }
  }

  return indices;
}
