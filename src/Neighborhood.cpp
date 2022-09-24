#include <cmath>

#include "Neighborhood.hpp"

NeighborhoodRadius::NeighborhoodRadius(float initRadius, int maxSteps)
    : init(initRadius)
    , tmax(maxSteps)
{
}

float NeighborhoodRadius::operator()(int t) const
{
    float const progress = static_cast<float>(t);
    float const remains = static_cast<float>(tmax - t);

    return init * expf(-progress * (logf(init) / remains));
}

Neighborhood::Neighborhood(NeighborhoodRadius radius)
    : radius(radius)
{
}
