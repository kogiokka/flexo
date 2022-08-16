#ifndef SELF_ORGANIZING_MAP_PANEL_H
#define SELF_ORGANIZING_MAP_PANEL_H

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "pane/PaneBase.hpp"
class SelfOrganizingMapPanel : public PaneBase
{
    wxButton* m_btnCreate;
    wxButton* m_btnStop;
    wxButton* m_btnRun;
    wxSlider* m_sldrNbhdRadius;
    wxStaticText* m_nbhdRadiusText;
    bool m_isStopped;

public:
    SelfOrganizingMapPanel(wxWindow* parent, WatermarkingProject& project);
    bool IsProjectStopped() const;

private:
    wxSizer* PopulateParametersPanel();
    wxSizer* PopulateDisplayPanel();
    wxSizer* PopulateControlPanel();
    void OnCreate(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnRun(wxCommandEvent& event);
    void OnMaxIterations(wxCommandEvent& event);
    void OnInitialLearningRate(wxCommandEvent& event);
    void OnInitialNeighborhoodRadius(wxCommandEvent& event);
    void OnNeighborhoodRadiusPreset(wxCommandEvent& event);
};

#endif
