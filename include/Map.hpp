#ifndef MAP_H
#define MAP_H

#include "Node.hpp"
#include "Vec.hpp"

#include <vector>

using MapFlags = int;

enum MapFlags_ : int {
    MapFlags_CyclicNone = 1 << 0,
    MapFlags_CyclicX = 1 << 1,
    MapFlags_CyclicY = 1 << 2,
};

typedef enum {
    MapInitState_Plane,
    MapInitState_Random,
} MapInitState;

struct Mesh;

template <int InDim, int OutDim>
struct Map {
    Vec<OutDim, int> size;
    std::vector<Node<InDim, OutDim>> nodes;
    MapFlags flags;
};

Mesh GenMapMesh(Map<3, 2> const& map);

#endif
