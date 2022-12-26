#ifndef MAP_H
#define MAP_H

#include "EditableMesh.hpp"
#include "Node.hpp"
#include "Vec.hpp"
#include "VecUtil.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "object/Object.hpp"

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
struct Map : public Object {
    Map() = default;
    virtual ~Map() = default;
    Vec<OutDim, int> size;
    std::vector<Node<InDim, OutDim>> nodes;
    MapFlags flags;

    void ApplyTransform() override;

private:
    Mesh GenerateMesh() const override;
    Wireframe GenerateWireMesh() const override;
};

EditableMesh GenMapEditableMesh(Map<3, 2> const& map);

template <int InDim, int OutDim>
Mesh Map<InDim, OutDim>::GenerateMesh() const
{
    return GenMapEditableMesh(*this).GenerateMesh();
}

template <int InDim, int OutDim>
Wireframe Map<InDim, OutDim>::GenerateWireMesh() const
{
    return GenMapEditableMesh(*this).GenerateWireframe();
}

template <int InDim, int OutDim>
void Map<InDim, OutDim>::ApplyTransform()
{
    auto mat = GenerateTransformStack().GenerateMatrix();
    for (auto& n : nodes) {
        n.weights = VECCONV(glm::vec3(mat * glm::vec4(VECCONV(n.weights), 1.0f)));
    }
    m_transform = Transform();
}

#endif
