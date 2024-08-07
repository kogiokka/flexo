#ifndef MAP_H
#define MAP_H

#include "Node.hpp"
#include "Vec.hpp"
#include "object/Object.hpp"

#include <string>
#include <vector>

using MapFlags = int;

enum MapInitState {
    MapInitState_Plane = 0,
    MapInitState_Cylinder,
    MapInitState_Random,
};

enum MapFlags_ : int {
    MapFlags_CyclicNone = 1 << 0,
    MapFlags_CyclicX = 1 << 1,
    MapFlags_CyclicY = 1 << 2,
};

class Graphics;
struct EditableMesh;

template <int InDim, int OutDim>
struct Map : public Object {
    Map();
    virtual ~Map() = default;
    Vec<OutDim, int> size;
    std::vector<Node<InDim, OutDim>> nodes;
    MapFlags flags;
    std::string textureName;

    template <typename... Params>
    Node<InDim, OutDim>& At(Params&&... coordinates);
    EditableMesh const& GetMesh() const;
    void GenerateDrawables(Graphics& gfx) override;
    void ApplyTransform() override;

private:
    void GenerateMesh();
};

#include "Map.cpp"

#endif
