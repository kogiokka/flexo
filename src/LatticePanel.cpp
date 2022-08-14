#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/valnum.h>

#include "Lattice.hpp"
#include "LatticePanel.hpp"
#include "Project.hpp"

enum {
    TextCtrl_LatticeWidth = wxID_HIGHEST + 200,
    TextCtrl_LatticeHeight,
    CheckBox_CyclicX,
    CheckBox_CyclicY,
    Button_Initialize,
};

wxDEFINE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

wxBEGIN_EVENT_TABLE(LatticePanel, wxScrolledWindow)
    EVT_TEXT(TextCtrl_LatticeWidth, LatticePanel::OnLatticeWidth)
    EVT_TEXT(TextCtrl_LatticeHeight, LatticePanel::OnLatticeHeight)
    EVT_CHECKBOX(CheckBox_CyclicX, LatticePanel::OnFlagCyclicX)
    EVT_CHECKBOX(CheckBox_CyclicY, LatticePanel::OnFlagCyclicY)
    EVT_BUTTON(Button_Initialize, LatticePanel::OnInitialize)
    EVT_UPDATE_UI(PANEL_LATTICE, LatticePanel::OnUpdateUI)
wxEND_EVENT_TABLE()

LatticePanel::LatticePanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           WatermarkingProject& project)
    : wxScrolledWindow(parent, id, pos, size, wxVSCROLL)
    , m_project(project)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto paneSizer = new wxFlexGridSizer(5, 2, 5, 16);

    layout->Add(paneSizer, wxSizerFlags().Expand().Border(wxALL, 10));

    wxSizerFlags labelFlags = wxSizerFlags().Right().CenterVertical();
    wxSizerFlags cellFlags = wxSizerFlags().Expand();

    wxIntegerValidator<int> validDimen;
    validDimen.SetRange(1, 512);

    m_tcLatticeWidth = new wxTextCtrl(this, TextCtrl_LatticeWidth, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxTE_CENTER, validDimen);
    m_tcLatticeHeight = new wxTextCtrl(this, TextCtrl_LatticeHeight, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxTE_CENTER, validDimen);
    *m_tcLatticeWidth << Lattice::Get(m_project).GetWidth();
    *m_tcLatticeHeight << Lattice::Get(m_project).GetHeight();

    auto chkBox1 = new wxCheckBox(this, CheckBox_CyclicX, "Cyclic on X");
    auto chkBox2 = new wxCheckBox(this, CheckBox_CyclicY, "Cyclic on Y");
    chkBox1->SetValue(false);
    chkBox2->SetValue(false);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Lattice Width", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(m_tcLatticeWidth, cellFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, "Lattice Height", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_ELLIPSIZE_END),
                   labelFlags);
    paneSizer->Add(m_tcLatticeHeight, cellFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(chkBox1, cellFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(chkBox2, cellFlags);

    paneSizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
                   labelFlags);
    paneSizer->Add(new wxButton(this, Button_Initialize, "Initialize"), wxSizerFlags().Expand());

    paneSizer->AddGrowableCol(0, 4);
    paneSizer->AddGrowableCol(1, 5);

    SetScrollRate(10, 10);
    SetSizer(layout);
}

void LatticePanel::OnLatticeWidth(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetWidth(tmp);
    }
}

void LatticePanel::OnLatticeHeight(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetHeight(tmp);
    }
}

void LatticePanel::OnFlagCyclicX(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicX);
    ProcessWindowEvent(event);
}

void LatticePanel::OnFlagCyclicY(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicY);
    ProcessWindowEvent(event);
}

void LatticePanel::OnInitialize(wxCommandEvent&)
{
    m_project.InitializeLattice();
    m_project.UpdateLatticeGraphics();
}

void LatticePanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!SelfOrganizingMap::Get(m_project).IsTraining());
}

