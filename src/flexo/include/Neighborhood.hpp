#ifndef NEIGHBORHOOD_H
#define NEIGHBORHOOD_H

#include "Vec.hpp"

struct NeighborhoodRadius {
    explicit NeighborhoodRadius(float initRadius = 0.0f, int maxSteps = 0);
    float operator()(int t) const;

    float init;
    int tmax;
};

struct Neighborhood {
    explicit Neighborhood(NeighborhoodRadius radius = NeighborhoodRadius());
    template <int OutDim>
    float operator()(int t, Vec<OutDim> coordBMU, Vec<OutDim> coordNode) const;

    NeighborhoodRadius radius;
};

template <int OutDim>
float Neighborhood::operator()(int t, Vec<OutDim> coordBMU, Vec<OutDim> coordNode) const
{
    float dist = 0.0f;
    float const r = radius(t);

    for (int i = 0; i < OutDim; i++) {
        float const diff = coordBMU[i] - coordNode[i];
        dist += diff * diff;
    }

    return expf(-dist / (2.0f * (r * r)));
}

#endif
