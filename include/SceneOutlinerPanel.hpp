#ifndef SCENE_OUTLINER_PANEL_H
#define SCENE_OUTLINER_PANEL_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>
#include <wx/window.h>

#include "pane/PaneBase.hpp"

wxDECLARE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

class SceneOutlinerPanel : public PaneBase
{
    wxSlider* m_slider;

public:
    SceneOutlinerPanel(wxWindow* parent, WatermarkingProject& project);

private:
    void OnCheckboxModel(wxCommandEvent& event);
    void OnCheckboxLatticeVertex(wxCommandEvent& event);
    void OnCheckboxLatticeEdge(wxCommandEvent& event);
    void OnCheckboxLatticeFace(wxCommandEvent& event);
    void OnCheckboxLightSource(wxCommandEvent& event);
    void OnSliderTransparency(wxCommandEvent& event);
};

#endif
