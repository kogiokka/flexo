#include "Node.hpp"

Node::Node(int x, int y, std::vector<float> init_weights)
  : m_x(x)
  , m_y(y)
{
  m_weights = init_weights;
}

int&
Node::x()
{
  return m_x;
}

int&
Node::y()
{
  return m_y;
}

int
Node::x() const
{
  return m_x;
}

int
Node::y() const
{
  return m_y;
}

float&
Node::operator[](int index)
{
  return m_weights[index];
}

float
Node::operator[](int index) const
{
  return m_weights[index];
}

int
Node::dimen() const
{
  return m_weights.size();
}

std::vector<float>&
Node::weights()
{
  return m_weights;
}

std::vector<float> const&
Node::weights() const
{
  return m_weights;
}
