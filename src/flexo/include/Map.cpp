#include "object/Map.hpp"
#include "EditableMesh.hpp"
#include "VecUtil.hpp"

template <int InDim, int OutDim>
EditableMesh const& Map<InDim, OutDim>::GetMesh() const
{
    return m_mesh;
}

template <int InDim, int OutDim>
Map<InDim, OutDim>::Map()
    : Object(ObjectType_Map)
{
}

template <int InDim, int OutDim>
void Map<InDim, OutDim>::GenerateMesh()
{
    EditableMesh mesh;

    int const width = size.x;
    int const height = size.y;

    for (auto const& node : nodes) {
        mesh.positions.push_back(VECCONV(node.weights));
        mesh.textureCoords.push_back(VECCONV(node.uv));
    }

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            unsigned int const idx = y * width + x;

            /**
             *  4-----3
             *  |     |
             *  |     |
             *  1-----2
             */
            unsigned int i1, i2, i3, i4;
            i1 = idx;
            i2 = idx + 1;
            i3 = idx + width + 1;
            i4 = idx + width;

            mesh.faces.push_back({ i1, i2, i3, i4 });
        }
    }

    m_mesh = mesh;
}

template <int InDim, int OutDim>
void Map<InDim, OutDim>::ApplyTransform()
{
    auto mat = GenerateTransformStack().GenerateMatrix();
    for (auto& n : nodes) {
        n.weights = VECCONV(glm::vec3(mat * glm::vec4(VECCONV(n.weights), 1.0f)));
    }
    m_transform = Transform();

    GenerateMesh();
}
