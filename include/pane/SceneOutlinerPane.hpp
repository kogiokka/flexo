#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase
{
public:
    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);
};

#endif
