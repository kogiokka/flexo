#include "Overlays.hpp"
#include "Colors.hpp"
#include "TransformStack.hpp"
#include "WireDrawable.hpp"
#include "gfx/DrawableBase.hpp"

Overlays::Overlays(Graphics& gfx)
{
    auto fine = ConstructIntervaledGrid(180, 18);
    auto coarse = ConstructIntervaledGrid(18, 2);

    auto axisX = EditableMesh();
    axisX.positions.emplace_back(1.0f, 0.0f, 0.0f);
    axisX.positions.emplace_back(-1.0f, 0.0f, 0.0f);
    axisX.faces.push_back({ 0, 1 });

    auto axisY = EditableMesh();
    axisY.positions.emplace_back(0.0f, 1.0f, 0.0f);
    axisY.positions.emplace_back(0.0f, -1.0f, 0.0f);
    axisY.faces.push_back({ 0, 1 });

    TransformStack ts;
    ts.PushScale(90.0f, 90.0f, 1.0f);
    ts.Apply(fine.positions);
    ts.Apply(coarse.positions);
    ts.Apply(axisX.positions);
    ts.Apply(axisY.positions);

    m_gridFine = std::make_shared<WireDrawable>(gfx, fine.GenerateWireframe());
    m_gridFine->SetColor(DARK_GREY);

    m_gridCoarse = std::make_shared<WireDrawable>(gfx, coarse.GenerateWireframe());
    m_gridCoarse->SetColor(GREY);

    m_axisX = std::make_shared<WireDrawable>(gfx, axisX.GenerateWireframe());
    m_axisX->SetColor(RED);

    m_axisY = std::make_shared<WireDrawable>(gfx, axisY.GenerateWireframe());
    m_axisY->SetColor(GREEN);

    m_flags = Overlays_GuidesAxisX | Overlays_GuidesAxisY | Overlays_GuidesGrid;
}

void Overlays::Submit(Renderer& renderer)
{
    if (m_flags & Overlays_GuidesAxisX) {
        m_axisX->SetColor(RED);
        m_axisX->Submit(renderer);
    }

    if (m_flags & Overlays_GuidesAxisY) {
        m_axisY->SetColor(GREEN);
        m_axisY->Submit(renderer);
    }

    if (m_flags & Overlays_GuidesGrid) {
        m_gridFine->Submit(renderer);
        m_gridCoarse->Submit(renderer);

        if (!(m_flags & Overlays_GuidesAxisX)) {
            m_axisX->SetColor(GREY);
        }
        if (!(m_flags & Overlays_GuidesAxisY)) {
            m_axisY->SetColor(GREY);
        }
    }
}

void Overlays::SetFlags(OverlayFlags flags)
{
    m_flags = flags;
}
