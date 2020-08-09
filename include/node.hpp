#pragma once

#include <glm/glm.hpp>

#include <vector>

class Node
{
  int m_x;
  int m_y;
  std::vector<float> m_weights;

public:
  Node(int x, int y, std::vector<float> init_weights);
  int& x();
  int& y();
  int x() const;
  int y() const;
  float& operator[](int index);
  float operator[](int index) const;
  int dimen() const;
  std::vector<float>& weights();
  std::vector<float> const& weights() const;
};
