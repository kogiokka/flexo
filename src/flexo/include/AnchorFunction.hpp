#ifndef ANCHOR_FUNCTION_H
#define ANCHOR_FUNCTION_H

#include <cmath>

#include "Vec.hpp"

struct AnchorFunction {
    explicit AnchorFunction(float radius = 0.0f)
        : radius(radius)
    {
    }

    template <int OutDim>
    float operator()(Vec<OutDim> coordAnchor, Vec<OutDim> coordNode) const;

    float radius;
};

template <int OutDim>
float AnchorFunction::operator()(Vec<OutDim> coordAnchor, Vec<OutDim> coordNode) const
{
    float dist = 0.0f;

    for (int i = 0; i < OutDim; i++) {
        float const diff = coordAnchor[i] - coordNode[i];
        dist += diff * diff;
    }

    return dist / radius * radius;
}

#endif
