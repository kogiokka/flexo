#ifndef SCENE_OUTLINER_PANEL_H
#define SCENE_OUTLINER_PANEL_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/slider.h>
#include <wx/window.h>

#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase
{
    wxSlider* m_slider;

public:
    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnSliderTransparency(wxCommandEvent& event);
};

#endif
