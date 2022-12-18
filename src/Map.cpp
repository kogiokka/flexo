#include "Map.hpp"
#include "EditableMesh.hpp"
#include "VecUtil.hpp"

EditableMesh GenMapEditableMesh(Map<3, 2> const& map)
{
    EditableMesh mesh;

    int const width = map.size.x;
    int const height = map.size.y;

    for (auto const& node : map.nodes) {
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

            mesh.faces.emplace_back(i1, i2, i3);
            mesh.faces.emplace_back(i1, i3, i4);
        }
    }

    return mesh;
}
