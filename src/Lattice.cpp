#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

#include "Lattice.hpp"
#include "RandomRealNumber.hpp"
#include "common/Logger.hpp"

Lattice::Lattice(int width, int height)
    : width(width)
    , height(height)
    , flags(LatticeFlags_CyclicNone)
{
    RandomRealNumber<float> rng(-10.0f, 10.0f);

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            neurons.emplace_back(i, j, rng.vector(3));
        }
    }
}
