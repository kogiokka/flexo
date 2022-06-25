#include "Node.hpp"

Node::Node(int x, int y, std::vector<float> initWeights)
    : m_x(x)
    , m_y(y)
{
    m_weights = initWeights;
}

Node::Node(Node const& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_weights = other.m_weights;
}

Node& Node::operator=(Node const& other)
{
    m_weights = other.m_weights;
    return *this;
}

int& Node::X()
{
    return m_x;
}

int& Node::Y()
{
    return m_y;
}

int Node::X() const
{
    return m_x;
}

int Node::Y() const
{
    return m_y;
}

float& Node::operator[](int index)
{
    return m_weights[index];
}

float Node::operator[](int index) const
{
    return m_weights[index];
}

int Node::Dimension() const
{
    return m_weights.size();
}

std::vector<float>& Node::Weights()
{
    return m_weights;
}

std::vector<float> const& Node::Weights() const
{
    return m_weights;
}
