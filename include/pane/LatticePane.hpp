#ifndef LATTICE_PANE_H
#define LATTICE_PANE_H

#include "pane/ControlsPaneBase.hpp"

class LatticePane : public ControlsPaneBase
{
public:
    LatticePane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnLatticeWidth(wxCommandEvent& event);
    void OnLatticeHeight(wxCommandEvent& event);
    void OnInitialize(wxCommandEvent& event);
};

#endif
