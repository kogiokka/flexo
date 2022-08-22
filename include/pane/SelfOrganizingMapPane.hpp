#ifndef SELF_ORGANIZING_MAP_PANE_H
#define SELF_ORGANIZING_MAP_PANE_H

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "Lattice.hpp"
#include "pane/ControlsPaneBase.hpp"

class SelfOrganizingMapPane : public ControlsPaneBase
{
    wxButton* m_btnCreate;
    wxButton* m_btnStop;
    wxButton* m_btnRun;
    wxSlider* m_sldrNbhdRadius;
    bool m_isStopped;

public:
    SelfOrganizingMapPane(wxWindow* parent, WatermarkingProject& project);
    bool IsProjectStopped() const;

private:
    void PopulateLatticePanel();
    void PopulateParametersPanel();
    void PopulateDisplayPanel();
    void PopulateControlPanel();
    void OnCreate(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnRun(wxCommandEvent& event);
    void OnMaxIterations(wxCommandEvent& event);
    void OnInitialLearningRate(wxCommandEvent& event);
    void SetupNeighborhoodRadiusSlider(float maxValue, float value);
};

#endif
