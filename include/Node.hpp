#ifndef NODE_H
#define NODE_H

#include "Vec.hpp"

template <int InDim, int OutDim>
struct Node {
    Vec<InDim> weights;
    Vec<OutDim> coords;

    Node(Vec<InDim> weights, Vec<OutDim> coordinates);
    Node(Node const& other);
    Node& operator=(Node const& other);

    float X() const;
    float Y() const;
    float Z() const;
};

template <int InDim, int OutDim>
Node<InDim, OutDim>::Node(Vec<InDim> weights, Vec<OutDim> coordinates)
    : weights(weights)
    , coords(coordinates)
{
}

template <int InDim, int OutDim>
Node<InDim, OutDim>::Node(Node const& other)
{
    weights = other.weights;
    coords = other.coords;
}

template <int InDim, int OutDim>
Node<InDim, OutDim>& Node<InDim, OutDim>::operator=(Node<InDim, OutDim> const& other)
{
    weights = other.weights;
    coords = other.coords;
    return *this;
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::X() const
{
    return coords[0];
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::Y() const
{
    return coords[1];
}

template <int InDim, int OutDim>
float Node<InDim, OutDim>::Z() const
{
    return coords[2];
}

#endif
