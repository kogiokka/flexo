#include <array>
#include <utility>

#include "gfx/EditableMesh.hpp"
#include "VecUtil.hpp"
#include "object/Map.hpp"

template <int InDim, int OutDim>
template <typename... Params>
Node<InDim, OutDim>& Map<InDim, OutDim>::At(Params&&... coordinates)
{
    static_assert(sizeof...(Params) == OutDim);

    std::array<int, OutDim> const coords { std::forward<Params>(coordinates)... };
    std::array<unsigned int, OutDim> mul { 1 };
    unsigned int index = coords[0];

    for (unsigned int i = 1; i < OutDim; i++) {
        mul[i] = mul[i - 1] * size[i - 1];
        index += mul[i] * coords[i];
    }

    return nodes[index];
}

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
void Map<InDim, OutDim>::GenerateDrawables(Graphics& gfx)
{
    GenerateMesh();

    if (!m_texture) {
        m_texture = Bind::TextureManager::Resolve(gfx, "images/blank.png", 0);
    }
    auto m = m_mesh.GenerateMesh();
    m_solid = std::make_shared<SolidDrawable>(gfx, m);
    m_textured = std::make_shared<TexturedDrawable>(gfx, m_mesh.GenerateMesh(), m_texture);
    m_wire = std::make_shared<WireDrawable>(gfx, m_mesh.GenerateWireframe());
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
