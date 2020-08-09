#pragma once

#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "node.hpp"
#include "random_vec3.hpp"

class Lattice
{
  int m_size;
  int m_currIteration;
  int m_leftIteration;
  float m_timeConstant;
  float m_currRate;
  float m_beginRate;
  float m_neighborhoodRadius;
  float m_error;
  std::vector<Node> m_neurons;

public:
  Lattice(int size, int iterations, float rate);
  bool input(std::vector<float> in);
  bool isFinished() const;
  int iterations() const;
  float learningRate() const;
  float neighborhoodRadius() const;
  float error() const;
  std::vector<Node> const& neurons() const;
  std::vector<float> vertexBuffer() const;
};
