#ifndef SCENE_OUTLINER_PANEL_H
#define SCENE_OUTLINER_PANEL_H

#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase
{
public:
    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);
};

#endif
