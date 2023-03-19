#include <memory>

#include "TransformStack.hpp"
#include "WireDrawable.hpp"
#include "gfx/DrawableBase.hpp"
#include "object/Guides.hpp"

static EditableMesh ConstructGuides(int numCell, int numBlock, float size = 2.0f);

Guides::Guides()
{
    SetViewFlags(ObjectViewFlag_Wire);

    m_fine = ConstructGuides(180, 18);
    m_coarse = ConstructGuides(18, 2);

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
    ts.Apply(m_fine.positions);
    ts.Apply(m_coarse.positions);
    ts.Apply(m_xAxis.positions);
    ts.Apply(m_yAxis.positions);
}

EditableMesh ConstructGuides(int numCell, int numBlock, float size)
{
    EditableMesh mesh;
    float blkSize = numCell / numBlock;
    float const cellSize = size / numCell;

    unsigned int begin = 0;
    unsigned int index = 0;
    for (int i = 0; i < numBlock; i++) {
        for (int j = 0; j < blkSize - 1; j++) {
            // X
            mesh.positions.emplace_back(+1.0f, -1.0f + (j + 1 + begin) * cellSize, 0.0f);
            mesh.positions.emplace_back(-1.0f, -1.0f + (j + 1 + begin) * cellSize, 0.0f);
            // Y
            mesh.positions.emplace_back(-1.0f + (j + 1 + begin) * cellSize, +1.0f, 0.0f);
            mesh.positions.emplace_back(-1.0f + (j + 1 + begin) * cellSize, -1.0f, 0.0f);

            mesh.faces.push_back({ index + 0, index + 1 });
            mesh.faces.push_back({ index + 2, index + 3 });
            index += 4;
        }
        begin += blkSize;
    }
    return mesh;
}

void Guides::GenerateDrawables(Graphics& gfx)
{
    {
        auto wire = std::make_shared<WireDrawable>(gfx, m_fine.GenerateWireframe());
        wire->SetColor(0.3f, 0.3f, 0.3f);
        wire->SetVisible(m_isVisible);
        m_drawlist.push_back(wire);
    }
    {
        auto wire = std::make_shared<WireDrawable>(gfx, m_coarse.GenerateWireframe());
        wire->SetColor(0.42f, 0.42f, 0.42f);
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

void Guides::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_fine.positions);
    st.Apply(m_xAxis.positions);
    st.Apply(m_yAxis.positions);

    m_transform = Transform();
}
