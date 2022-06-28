#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

#include "Lattice.hpp"
#include "RandomRealNumber.hpp"
#include "common/Logger.hpp"

Lattice::Lattice(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_flags(LatticeFlags_CyclicNone)
{
    RandomRealNumber<float> rng(-1.0f, 1.0f);

    for (int j = 0; j < m_height; ++j) {
        for (int i = 0; i < m_width; ++i) {
            m_neurons.emplace_back(i, j, rng.vector(3));
        }
    }
}

int Lattice::Width() const
{
    return m_width;
}

int Lattice::Height() const
{
    return m_height;
}

std::vector<Node> const& Lattice::Neurons() const
{
    return m_neurons;
}

void Lattice::SetFlags(LatticeFlags flags)
{
    m_flags = flags;
}
