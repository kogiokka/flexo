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

struct Lattice
{
    int width;
    int height;
    LatticeFlags flags;
    std::vector<Node> neurons;

    Lattice(int width, int height);
};

#endif
