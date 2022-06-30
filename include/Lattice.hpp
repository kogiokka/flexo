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
    LatticeFlags_CyclicNone = 1 << 0,
    LatticeFlags_CyclicX = 1 << 1,
    LatticeFlags_CyclicY = 1 << 2,
};

struct Lattice {
    int width;
    int height;
    LatticeFlags flags;
    std::vector<Node> neurons;

    Lattice(int width, int height);
};

#endif
