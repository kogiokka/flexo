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
Lattice::input(std::vector<float> in)
{
  if (iterRemained_ <= 0)
    return false;

  int const iterCurrent = (iterCap_ - iterRemained_);
  neighborhoodRadius_ = dimen_ * exp(-iterCurrent / timeConstant_);

  std::unique_ptr<Node> bmu;
  float dist_min = std::numeric_limits<float>::max();

  // Find Best Matching Unit
  for (Node& node : neurons_) {
    float sum = 0;
    for (int i = 0; i < node.dimen(); ++i) {
      float const diff = node[i] - in[i];
      sum += diff * diff;
    }
    if (dist_min > sum) {
      bmu = std::make_unique<Node>(node);
      dist_min = sum;
    }
  }

  for (Node& node : neurons_) {
    float const dx = bmu->x() - node.x();
    float const dy = bmu->y() - node.y();
    float const dist_to_bmu_sq = dx * dx + dy * dy;
    float const radius_sq = (neighborhoodRadius_ * neighborhoodRadius_);
    if (dist_to_bmu_sq < radius_sq) {
      float const influence = expf(-dist_to_bmu_sq / (2.0f * radius_sq)); // Why does (2.0f * radius_sq) need parens?
      for (int i = 0; i < node.dimen(); ++i) {
        node[i] += rateCurrent_ * influence * (in[i] - node[i]);
      }
    }
  }

  rateCurrent_ = rateInitial_ * expf(-iterCurrent / iterRemained_); // iteration_left_ > 0

  --iterRemained_;

  return true;
}

int
Lattice::dimension() const
{
  return dimen_;
}

int
Lattice::iterationCap() const
{
  return iterCap_;
}

int
Lattice::currentIteration() const
{
  return iterCap_ - iterRemained_;
}

float
Lattice::neighborhoodRadius() const
{
  return neighborhoodRadius_;
}

std::vector<Node> const&
Lattice::neurons() const
{
  return neurons_;
}

float
Lattice::initialRate() const
{
  return rateInitial_;
}

float
Lattice::currentRate() const
{
  return rateCurrent_;
}

std::vector<unsigned int>
Lattice::edgeIndices() const
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
Lattice::faceIndices() const
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
