#ifndef SELF_ORGANIZING_MAP_PANE_H
#define SELF_ORGANIZING_MAP_PANE_H

#include <wx/button.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_SOM_PANE_MAP_CHANGED, wxCommandEvent);

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
    void PopulateMapPanel();
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
