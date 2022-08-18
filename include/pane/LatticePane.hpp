#ifndef LATTICE_PANE_H
#define LATTICE_PANE_H

#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

class LatticePane : public ControlsPaneBase
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

