#ifndef MAP_H
#define MAP_H

#include "Node.hpp"
#include "Vec.hpp"
#include "object/Object.hpp"

#include <vector>

using MapFlags = int;

enum MapFlags_ : int {
    MapFlags_CyclicNone = 1 << 0,
    MapFlags_CyclicX = 1 << 1,
    MapFlags_CyclicY = 1 << 2,
};

class Graphics;
struct EditableMesh;

template <int InDim, int OutDim>
struct Map : public Object {
    Map() = default;
    virtual ~Map() = default;
    Vec<OutDim, int> size;
    std::vector<Node<InDim, OutDim>> nodes;
    MapFlags flags;

    EditableMesh const& GetMesh() const;
    void GenerateMesh();
    void ApplyTransform() override;
};

#include "Map.cpp"

#endif
