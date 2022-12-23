#include <memory>

#include "TransformStack.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "object/Guides.hpp"

Guides::Guides()
{
    SetViewFlags(ObjectViewFlag_Wire);

    m_grid = EditableMesh();
    unsigned int quadSize = 90;
    float const diff = 1.0f / static_cast<float>(quadSize);
    for (unsigned int i = 0; i < quadSize; i++) {
        // X
        m_grid.positions.emplace_back(+1.0f, -1.0f + i * diff, 0.0f);
        m_grid.positions.emplace_back(-1.0f, -1.0f + i * diff, 0.0f);
        m_grid.positions.emplace_back(+1.0f, -1.0f + (i + quadSize + 1) * diff, 0.0f);
        m_grid.positions.emplace_back(-1.0f, -1.0f + (i + quadSize + 1) * diff, 0.0f);

        // Y
        m_grid.positions.emplace_back(-1.0f + i * diff, +1.0f, 0.0f);
        m_grid.positions.emplace_back(-1.0f + i * diff, -1.0f, 0.0f);
        m_grid.positions.emplace_back(-1.0f + (i + quadSize + 1) * diff, +1.0f, 0.0f);
        m_grid.positions.emplace_back(-1.0f + (i + quadSize + 1) * diff, -1.0f, 0.0f);

        for (unsigned int j = i * 8; j < (i + 1) * 8; j += 2) {
            m_grid.faces.push_back({ j, j + 1 });
        }
    }

    m_xAxis = EditableMesh();
    m_xAxis.positions.emplace_back(1.0f, 0.0f, 0.0f);
    m_xAxis.positions.emplace_back(-1.0f, 0.0f, 0.0f);
    m_xAxis.faces.push_back({ 0, 1 });

    m_yAxis = EditableMesh();
    m_yAxis.positions.emplace_back(0.0f, 1.0f, 0.0f);
    m_yAxis.positions.emplace_back(0.0f, -1.0f, 0.0f);
    m_yAxis.faces.push_back({ 0, 1 });

    TransformStack ts;
    ts.PushScale(90.0f, 90.0f, 1.0f);
    ts.Apply(m_grid);
    ts.Apply(m_xAxis);
    ts.Apply(m_yAxis);
}

void Guides::GenerateDrawables(Graphics& gfx)
{
    {
        auto wire = std::make_shared<WireDrawable>(gfx, m_grid.GenerateWireframe());
        wire->SetColor(0.3f, 0.3f, 0.3f);
        wire->SetVisible(m_isVisible);
        m_drawlist.push_back(wire);
    }
    {
        auto axis = std::make_shared<WireDrawable>(gfx, m_xAxis.GenerateWireframe());
        axis->SetColor(0.8f, 0.3f, 0.3f);
        axis->SetVisible(m_isVisible);
        m_drawlist.push_back(axis);
    }
    {
        auto axis = std::make_shared<WireDrawable>(gfx, m_yAxis.GenerateWireframe());
        axis->SetColor(0.3f, 0.8f, 0.3f);
        axis->SetVisible(m_isVisible);
        m_drawlist.push_back(axis);
    }
}

Object::DrawList const& Guides::GetDrawList()
{

    return m_drawlist;
}

Mesh Guides::GenerateMesh() const
{
    return {};
}

Wireframe Guides::GenerateWireMesh() const
{
    return {};
}
