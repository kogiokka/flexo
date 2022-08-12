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
    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);
    auto row4 = new wxBoxSizer(wxHORIZONTAL);
    auto row5 = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags labelFlags = wxSizerFlags().Center().Proportion(4).Border(wxRIGHT, 10);
    wxSizerFlags textCtrlFlags = wxSizerFlags().Expand().Proportion(5);
    wxSizerFlags rowFlags = wxSizerFlags().Expand();

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

    row1->Add(new wxStaticText(this, wxID_ANY, "Lattice Width", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
              labelFlags);
    row1->Add(m_tcLatticeWidth, textCtrlFlags);
    row2->Add(new wxStaticText(this, wxID_ANY, "Lattice Height", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT),
              labelFlags);
    row2->Add(m_tcLatticeHeight, textCtrlFlags);
    row3->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    row3->Add(chkBox1, textCtrlFlags);
    row4->Add(new wxStaticText(this, wxID_ANY, ""), labelFlags);
    row4->Add(chkBox2, textCtrlFlags);
    auto btn = new wxButton(this, Button_Initialize, "Initialize");
    row5->Add(btn, wxSizerFlags().Expand());

    layout->Add(row1, rowFlags.Border(wxTOP | wxLEFT | wxRIGHT, 10));
    layout->Add(row2, rowFlags.Border(wxLEFT | wxRIGHT, 10));
    layout->Add(row3, rowFlags.Border(wxLEFT | wxRIGHT, 10));
    layout->Add(row4, rowFlags.Border(wxLEFT | wxRIGHT, 10));
    layout->Add(row5, rowFlags.Border(wxBOTTOM | wxLEFT | wxRIGHT, 10));

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
