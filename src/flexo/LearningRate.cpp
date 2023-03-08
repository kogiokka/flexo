#include <cmath>

#include "LearningRate.hpp"

LearningRate::LearningRate(float initialRate, int maxSteps)
    : init(initialRate)
    , tmax(maxSteps)
{
}

float LearningRate::operator()(int t) const
{
    float const progress = static_cast<float>(t);
    float const remains = static_cast<float>(tmax - t);
    return init * expf(-progress * 1.0f / remains);
}
