#ifndef SELF_ORGANIZING_MAP_PANEL_H
#define SELF_ORGANIZING_MAP_PANEL_H

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

enum {
    PANEL_SOM = wxID_HIGHEST + 1,
};

class WatermarkingProject;

class SelfOrganizingMapPanel : public wxScrolledWindow
{
    wxButton* m_btnCreate;
    wxButton* m_btnStop;
    wxButton* m_btnRun;
    wxTextCtrl* m_tcIterations;
    wxTextCtrl* m_tcNbhdRadius;
    wxTextCtrl* m_tcLearnRate;
    wxSlider* m_sldrNbhdRadius;
    wxStaticText* m_nbhdRadiusText;
    bool m_isStopped;

public:
    SelfOrganizingMapPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           WatermarkingProject& project);
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
    void OnUpdateUI(wxUpdateUIEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
