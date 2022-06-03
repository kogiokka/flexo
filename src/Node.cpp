#include "Node.hpp"

Node::Node(int x, int y, std::vector<float> initWeights)
    : x_(x)
    , y_(y)
{
    weights_ = initWeights;
}

Node::Node(Node const& other)
{
    x_ = other.x_;
    y_ = other.y_;
    weights_ = other.weights_;
}

Node& Node::operator=(Node const& other)
{
    weights_ = other.weights_;
    return *this;
}

int& Node::X()
{
    return x_;
}

int& Node::Y()
{
    return y_;
}

int Node::X() const
{
    return x_;
}

int Node::Y() const
{
    return y_;
}

float& Node::operator[](int index)
{
    return weights_[index];
}

float Node::operator[](int index) const
{
    return weights_[index];
}

int Node::Dimension() const
{
    return weights_.size();
}

std::vector<float>& Node::Weights()
{
    return weights_;
}

std::vector<float> const& Node::Weights() const
{
    return weights_;
}
