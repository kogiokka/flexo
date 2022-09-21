#ifndef NODE_H
#define NODE_H

#include <array>

template <int S>
class Node
{
    int m_x;
    int m_y;
    std::array<float, S> m_weights;

public:
    Node(int x, int y, std::array<float, S> initWeights);
    Node(Node const& other);
    int& X();
    int& Y();
    int X() const;
    int Y() const;
    Node& operator=(Node const& other);
    float& operator[](int index);
    float operator[](int index) const;
    int Dimensions() const;
    std::array<float, S>& Weights();
    std::array<float, S> const& Weights() const;
};

template <int S>
Node<S>::Node(int x, int y, std::array<float, S> initWeights)
    : m_x(x)
    , m_y(y)
{
    m_weights = initWeights;
}

template <int S>
Node<S>::Node(Node const& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_weights = other.m_weights;
}

template <int S>
Node<S>& Node<S>::operator=(Node<S> const& other)
{
    m_weights = other.m_weights;
    return *this;
}

template <int S>
int& Node<S>::X()
{
    return m_x;
}

template <int S>
int& Node<S>::Y()
{
    return m_y;
}

template <int S>
int Node<S>::X() const
{
    return m_x;
}

template <int S>
int Node<S>::Y() const
{
    return m_y;
}

template <int S>
float& Node<S>::operator[](int index)
{
    return m_weights[index];
}

template <int S>
float Node<S>::operator[](int index) const
{
    return m_weights[index];
}

template <int S>
int Node<S>::Dimensions() const
{
    return S;
}

template <int S>
std::array<float, S>& Node<S>::Weights()
{
    return m_weights;
}

template <int S>
std::array<float, S> const& Node<S>::Weights() const
{
    return m_weights;
}

#endif
