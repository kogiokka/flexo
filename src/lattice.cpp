#include "lattice.hpp"

#include <imgui.h>

Lattice::Lattice(int size, int iterations, float rate)
  : m_size(size)
  , m_currIteration(0)
  , m_leftIteration(iterations)
  , m_currRate(rate)
  , m_beginRate(rate)
  , m_neighborhoodRadius(size)
  , m_error(0.0f)
{
  RandomVec3 rc;
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      auto const& w = rc.color();
      m_neurons.emplace_back(i, j, std::vector<float>(w.begin(), w.end()));
    }
  }
  m_timeConstant = iterations / log(size);
}

bool
Lattice::input(std::vector<float> in)
{
  if (m_leftIteration <= 0)
    return false;

  m_neighborhoodRadius = m_size * exp(-m_currIteration / m_timeConstant);

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

  m_currRate = m_beginRate * expf(-m_currIteration / m_leftIteration); // iteration_left_ > 0

  ++m_currIteration;
  --m_leftIteration;

  return true;
}

float
Lattice::error() const
{
  return m_error;
}

int
Lattice::iterations() const
{
  return m_currIteration;
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
  return (m_leftIteration <= 0);
}

std::vector<float>
Lattice::vertexBuffer() const
{
  std::vector<int> indices;
  std::vector<float> buffer;
  buffer.reserve(5 * m_neurons.size());

  for (int i = 0; i < m_size - 1; ++i) {
    for (int j = 0; j < m_size - 1; ++j) {
      indices.push_back(i * m_size + j);
      indices.push_back((i + 1) * m_size + j);
      indices.push_back(i * m_size + j + 1);
      indices.push_back(i * m_size + j + 1);
      indices.push_back((i + 1) * m_size + j);
      indices.push_back((i + 1) * m_size + j + 1);
    }
  }
  for (int i : indices) {
    auto const& n = m_neurons[i];
    buffer.push_back(n.x());
    buffer.push_back(n.y());
    buffer.push_back(n[0]);
    buffer.push_back(n[1]);
    buffer.push_back(n[2]);
  }
  return buffer;
}
