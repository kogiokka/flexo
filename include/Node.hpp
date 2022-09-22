#ifndef NODE_H
#define NODE_H

#include <array>

template <int InDim, int OutDim>
struct Node {
    std::array<float, InDim> mWeights;
    std::array<float, OutDim> mCoordinates;

    Node(std::array<float, InDim> weights, std::array<float, OutDim> coordinates);
    Node(Node const& other);
    Node& operator=(Node const& other);

    float X() const;
    float Y() const;
    float Z() const;
};

template <int InDim, int OutDim>
Node<InDim, OutDim>::Node(std::array<float, InDim> weights, std::array<float, OutDim> coordinates)
    : mWeights(weights)
    , mCoordinates(coordinates)
{
}

template <int InDim, int OutDim>
Node<InDim, OutDim>::Node(Node const& other)
{
    mWeights = other.mWeights;
    mCoordinates = other.mCoordinates;
}

template <int InDim, int OutDim>
Node<InDim, OutDim>& Node<InDim, OutDim>::operator=(Node<InDim, OutDim> const& other)
{
    mWeights = other.mWeights;
    mCoordinates = other.mCoordinates;
    return *this;
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::X() const
{
    return mCoordinates[0];
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::Y() const
{
    return mCoordinates[1];
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::Z() const
{
    return mCoordinates[2];
}

#endif
