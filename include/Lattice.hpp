#ifndef LATTICE_H
#define LATTICE_H

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "InputData.hpp"
#include "Node.hpp"

using LatticeFlags = int;

enum LatticeFlags_ : int {
    LatticeFlags_CyclicNone = 0,
    LatticeFlags_CyclicX,
    LatticeFlags_CyclicY,
};

class SelfOrganizingMap;

class Lattice
{
    friend SelfOrganizingMap;
    int m_width;
    int m_height;
    LatticeFlags m_flags;
    std::vector<Node> m_neurons;

public:
    Lattice(int width, int height);
    int Width() const;
    int Height() const;
    std::vector<Node> const& Neurons() const;
    void SetFlags(LatticeFlags flags);
};

#endif
