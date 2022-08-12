#ifndef LATTICE_PANEL_H
#define LATTICE_PANEL_H

#include <wx/scrolwin.h>
#include <wx/textctrl.h>

enum {
    PANEL_LATTICE = wxID_HIGHEST + 40, // FIXME
};

wxDECLARE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

class WatermarkingProject;

class LatticePanel : public wxScrolledWindow
{
    wxTextCtrl* m_tcLatticeWidth;
    wxTextCtrl* m_tcLatticeHeight;

public:
    LatticePanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size, WatermarkingProject& project);

private:
    void OnLatticeWidth(wxCommandEvent& event);
    void OnLatticeHeight(wxCommandEvent& event);
    void OnFlagCyclicX(wxCommandEvent& event);
    void OnFlagCyclicY(wxCommandEvent& event);
    void OnInitialize(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
