#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "Lattice.hpp"
#include "Project.hpp"
#include "pane/LatticePane.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

LatticePane::LatticePane(wxWindow* parent, WatermarkingProject& project)
    : PaneBase(parent, project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto paneSizer = new wxFlexGridSizer(5, 2, 5, 16);

    layout->Add(paneSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    wxSizerFlags labelFlags = wxSizerFlags().Right().CenterVertical();
    wxSizerFlags ctrlFlags = wxSizerFlags().Expand();

    wxIntegerValidator<int> validDimen;
    validDimen.SetRange(1, 512);

    auto width
        = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validDimen);
    auto height
        = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validDimen);
    auto cyclicX = new wxCheckBox(this, wxID_ANY, "Cyclic on X");
    auto cyclicY = new wxCheckBox(this, wxID_ANY, "Cyclic on Y");
    auto initButton = new wxButton(this, wxID_ANY, "Initialize");

    *width << Lattice::Get(m_project).GetWidth();
    *height << Lattice::Get(m_project).GetHeight();
    cyclicX->SetValue(false);
    cyclicY->SetValue(false);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Lattice Width", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(width, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Lattice Height", wxDefaultPosition, wxDefaultSize,
                                    wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(height, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(cyclicX, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(cyclicY, ctrlFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(initButton, wxSizerFlags().Expand());

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    SetSizer(layout);

    Bind(wxEVT_UPDATE_UI,
         [this](wxUpdateUIEvent& event) { event.Enable(!SelfOrganizingMap::Get(m_project).IsTraining()); });
    width->Bind(wxEVT_TEXT, &LatticePane::OnLatticeWidth, this);
    height->Bind(wxEVT_TEXT, &LatticePane::OnLatticeHeight, this);
    cyclicX->Bind(wxEVT_CHECKBOX, &LatticePane::OnFlagCyclicX, this);
    cyclicY->Bind(wxEVT_CHECKBOX, &LatticePane::OnFlagCyclicY, this);
    initButton->Bind(wxEVT_BUTTON, &LatticePane::OnInitialize, this);
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

void LatticePane::OnFlagCyclicX(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicX);
    ProcessWindowEvent(event);
}

void LatticePane::OnFlagCyclicY(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicY);
    ProcessWindowEvent(event);
}

void LatticePane::OnInitialize(wxCommandEvent&)
{
    m_project.InitializeLattice();
    m_project.UpdateLatticeGraphics();
}
