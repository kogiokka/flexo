#include "Lattice.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

Lattice::Lattice(int width, int height, int iterations, float initRate)
  : width_(width)
  , height_(height)
  , lenDiag_(sqrtf(static_cast<float>(width * width + height * height)))
  , iterCap_(iterations)
  , iterRemained_(iterations)
  , rateCurrent_(initRate)
  , rateInitial_(initRate)
  , neighborhoodRadius_(lenDiag_)
  , RNG_(-1.0f, 1.0f)
{
  for (int j = 0; j < height_; ++j) {
    for (int i = 0; i < width_; ++i) {
      neurons_.emplace_back(i, j, RNG_.vector(3));
    }
  }
  timeConstant_ = iterations / logf(neighborhoodRadius_);
}

bool
Lattice::Input(std::vector<float> in)
{
  if (iterRemained_ <= 0)
    return false;

  float const progress = static_cast<float>(iterCap_ - iterRemained_);

  // Decay of neighborhood and learning rate
  neighborhoodRadius_ = lenDiag_ * expf(-progress / timeConstant_);
  rateCurrent_ = rateInitial_ * expf(-progress / iterRemained_);

  auto bmu = neurons_.cbegin();
  float distMin = std::numeric_limits<float>::max();

  // Find the Best Matching Unit
  for (auto it = neurons_.cbegin(); it != neurons_.cend(); ++it) {
    float sum = 0;
    for (int i = 0; i < it->dimension(); ++i) {
      float const diff = (*it)[i] - in[i];
      sum += diff * diff;
    }
    if (distMin > sum) {
      bmu = it;
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
Lattice::Width() const
{
  return width_;
}

int
Lattice::Height() const
{
  return height_;
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

  for (int i = 0; i < height_ - 1; ++i) {
    for (int j = 0; j < width_ - 1; ++j) {
      indices.push_back(i * width_ + j);
      indices.push_back(i * width_ + j + 1);
      indices.push_back(i * width_ + j + 1);
      indices.push_back((i + 1) * width_ + j + 1);
      indices.push_back((i + 1) * width_ + j + 1);
      indices.push_back((i + 1) * width_ + j);
      indices.push_back((i + 1) * width_ + j);
      indices.push_back(i * width_ + j);
    }
  }

  return indices;
}

std::vector<unsigned int>
Lattice::FaceIndices() const
{
  std::vector<unsigned int> indices;

  for (int i = 0; i < height_ - 1; ++i) {
    for (int j = 0; j < width_ - 1; ++j) {
      indices.push_back(i * width_ + j);
      indices.push_back((i + 1) * width_ + j);
      indices.push_back(i * width_ + j + 1);
      indices.push_back(i * width_ + j + 1);
      indices.push_back((i + 1) * width_ + j);
      indices.push_back((i + 1) * width_ + j + 1);
    }
  }

  return indices;
}
