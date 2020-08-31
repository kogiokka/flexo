#include "Lattice.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>

Lattice::Lattice(int dimen, int iterations, float learningRate)
  : m_dimen(dimen)
  , m_maxIter(iterations)
  , m_remainingIter(iterations)
  , m_currRate(learningRate)
  , m_initialRate(learningRate)
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

  m_currRate = m_initialRate * expf(-currentIter / m_remainingIter); // iteration_left_ > 0

  --m_remainingIter;

  return true;
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
Lattice::initialRate() const
{
  return m_initialRate;
}

float
Lattice::currentRate() const
{
  return m_currRate;
}

std::vector<unsigned int>
Lattice::edgeIndices() const
{
  std::vector<unsigned int> indices;

  for (int i = 0; i < m_dimen - 1; ++i) {
    for (int j = 0; j < m_dimen - 1; ++j) {
      indices.push_back(i * m_dimen + j);
      indices.push_back(i * m_dimen + j + 1);
      indices.push_back(i * m_dimen + j + 1);
      indices.push_back((i + 1) * m_dimen + j + 1);
      indices.push_back((i + 1) * m_dimen + j + 1);
      indices.push_back((i + 1) * m_dimen + j);
      indices.push_back((i + 1) * m_dimen + j);
      indices.push_back(i * m_dimen + j);
    }
  }

  return indices;
}

std::vector<unsigned int>
Lattice::faceIndices() const
{
  std::vector<unsigned int> indices;

  for (int i = 0; i < m_dimen - 1; ++i) {
    for (int j = 0; j < m_dimen - 1; ++j) {
      indices.push_back(i * m_dimen + j);
      indices.push_back((i + 1) * m_dimen + j);
      indices.push_back(i * m_dimen + j + 1);
      indices.push_back(i * m_dimen + j + 1);
      indices.push_back((i + 1) * m_dimen + j);
      indices.push_back((i + 1) * m_dimen + j + 1);
    }
  }

  return indices;
}
