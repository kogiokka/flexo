#ifndef SCENE_OUTLINER_PANEL_H
#define SCENE_OUTLINER_PANEL_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>

enum {
    Panel_SceneOutliner = wxID_HIGHEST + 50,
};

wxDECLARE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

class WatermarkingProject;

class SceneOutlinerPanel : public wxScrolledWindow
{
    wxSlider* m_slider;

public:
    SceneOutlinerPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                       WatermarkingProject& project);

private:
    void OnCheckboxModel(wxCommandEvent& event);
    void OnCheckboxLatticeVertex(wxCommandEvent& event);
    void OnCheckboxLatticeEdge(wxCommandEvent& event);
    void OnCheckboxLatticeFace(wxCommandEvent& event);
    void OnCheckboxLightSource(wxCommandEvent& event);
    void OnSliderTransparency(wxCommandEvent& event);

    WatermarkingProject& m_project;
    wxDECLARE_EVENT_TABLE();
};

#endif
