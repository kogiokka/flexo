#include <wx/valnum.h>

#include "Lattice.hpp"
#include "Project.hpp"
#include "pane/LatticePane.hpp"

LatticePane::LatticePane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* group = AddGroup("Properties", 5);

    wxIntegerValidator<int> validDimen;
    validDimen.SetRange(1, 512);

    auto* width = group->AddInputText("Lattice Width");
    auto* height = group->AddInputText("Lattice Height");
    auto* cyclicX = group->AddCheckBox("Cyclic on X", false);
    auto* cyclicY = group->AddCheckBox("Cyclic on Y", false);
    auto* initButton = group->AddButton("Initialize");

    *width << Lattice::Get(m_project).GetWidth();
    *height << Lattice::Get(m_project).GetHeight();

    width->Bind(wxEVT_TEXT, &LatticePane::OnLatticeWidth, this);
    height->Bind(wxEVT_TEXT, &LatticePane::OnLatticeHeight, this);
    cyclicX->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) {
        LatticeFlags flags = Lattice::Get(m_project).GetFlags() ^ LatticeFlags_CyclicX;
        Lattice::Get(m_project).SetFlags(flags);
    });
    cyclicY->Bind(wxEVT_CHECKBOX, [this](wxCommandEvent&) {
        LatticeFlags flags = Lattice::Get(m_project).GetFlags() ^ LatticeFlags_CyclicY;
        Lattice::Get(m_project).SetFlags(flags);
    });
    initButton->Bind(wxEVT_BUTTON, &LatticePane::OnInitialize, this);

    Bind(wxEVT_UPDATE_UI,
         [this](wxUpdateUIEvent& event) { event.Enable(!SelfOrganizingMap::Get(m_project).IsTraining()); });
}

void LatticePane::OnLatticeWidth(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetWidth(tmp);
    }
}

void LatticePane::OnLatticeHeight(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetHeight(tmp);
    }
}

void LatticePane::OnInitialize(wxCommandEvent&)
{
    m_project.InitializeLattice();
    m_project.UpdateLatticeGraphics();
}
