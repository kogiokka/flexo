#ifndef LATTICE_PANEL_H
#define LATTICE_PANEL_H

#include "pane/PaneBase.hpp"

wxDECLARE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

class LatticePane : public PaneBase
{
public:
    LatticePane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnLatticeWidth(wxCommandEvent& event);
    void OnLatticeHeight(wxCommandEvent& event);
    void OnFlagCyclicX(wxCommandEvent& event);
    void OnFlagCyclicY(wxCommandEvent& event);
    void OnInitialize(wxCommandEvent& event);
};

#endif

