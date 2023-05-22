#ifndef SELF_ORGANIZING_MAP_PANE_H
#define SELF_ORGANIZING_MAP_PANE_H

#include <wx/bmpcbox.h>
#include <wx/button.h>
#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_SOM_PANE_TARGET_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_SOM_PANE_MAP_ADDED, wxCommandEvent);
wxDECLARE_EVENT(EVT_SOM_PANE_MAP_DELETED, wxCommandEvent);

class SelfOrganizingMapPane : public ControlsPaneBase
{
    wxButton* m_btnCreate;
    wxButton* m_btnStop;
    wxButton* m_btnRun;
    wxButton* m_btnTexmap;
    wxTextCtrl* m_target;
    wxBitmapComboBox* m_mapCombo;
    wxSlider* m_sldrNbhdRadius;
    bool m_isStopped;

public:
    SelfOrganizingMapPane(wxWindow* parent, FlexoProject& project);
    bool IsProjectStopped() const;

private:
    void PopulateConfigPanel();
    void PopulateDisplayPanel();
    void PopulateControlPanel();
    void OnCreate(wxCommandEvent& event);
    void OnStop(wxCommandEvent& event);
    void OnRun(wxCommandEvent& event);
    void OnMaxIterations(wxCommandEvent& event);
    void OnInitialLearningRate(wxCommandEvent& event);
    void OnMapAdded(wxCommandEvent& event);
    void OnMapDeleted(wxCommandEvent& event);
    void OnTargetChanged(wxCommandEvent& event);
    void OnComboBox(wxCommandEvent& event);
    void SetupNeighborhoodRadiusSlider(float maxValue, float value);
};

#endif
