#include "lattice.hpp"

#include <cassert>

Lattice::Lattice(int dimen, int iterations, float learningRate)
  : m_dimen(dimen)
  , m_maxIter(iterations)
  , m_remainingIter(iterations)
  , m_currRate(learningRate)
  , m_beginRate(learningRate)
  , m_neighborhoodRadius(dimen)
  , m_random(-1.0f, 1.0f)
{
  for (int i = 0; i < dimen; ++i) {
    for (int j = 0; j < dimen; ++j) {
      auto w = m_random.vector(3);
      m_neurons.emplace_back(i, j, std::vector<float>(w.begin(), w.end()));
    }
  }
  m_timeConst = iterations / log(dimen);
}

#include <iostream>

bool
Lattice::input(std::vector<float> in)
{
  if (m_remainingIter <= 0)
    return false;

  int const currentIter = (m_maxIter - m_remainingIter);
  m_neighborhoodRadius = m_dimen * exp(-currentIter / m_timeConst);

  std::unique_ptr<Node> bmu;
  float dist_min = std::numeric_limits<float>::max();

  // Find Best Matching Unit
  for (Node& node : m_neurons) {
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

  for (Node& node : m_neurons) {
    float const dx = bmu->x() - node.x();
    float const dy = bmu->y() - node.y();
    float const dist_to_bmu_sq = dx * dx + dy * dy;
    float const radius_sq = (m_neighborhoodRadius * m_neighborhoodRadius);
    if (dist_to_bmu_sq < radius_sq) {
      float const influence = expf(-dist_to_bmu_sq / (2.0f * radius_sq)); // Why does (2.0f * radius_sq) need parens?
      for (int i = 0; i < node.dimen(); ++i) {
        node[i] += m_currRate * influence * (in[i] - node[i]);
      }
    }
  }

  m_currRate = m_beginRate * expf(-currentIter / m_remainingIter); // iteration_left_ > 0

  --m_remainingIter;

  return true;
}

void
Lattice::setIterations(unsigned int num)
{
  m_maxIter = num;
  m_remainingIter = num;
  m_timeConst = m_remainingIter / log(m_dimen);
}

int
Lattice::dimension() const
{
  return m_dimen;
}

int
Lattice::maxIterations() const
{
  return m_maxIter;
}

int
Lattice::currentIteration() const
{
  return m_maxIter - m_remainingIter;
}

float
Lattice::neighborhoodRadius() const
{
  return m_neighborhoodRadius;
}

std::vector<Node> const&
Lattice::neurons() const
{
  return m_neurons;
}

float
Lattice::learningRate() const
{
  return m_currRate;
}

bool
Lattice::isFinished() const
{
  return (m_remainingIter <= 0);
}
