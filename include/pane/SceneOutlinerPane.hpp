#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include <wx/treelist.h>

#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase
{
public:
    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);

private:
    wxTreeListCtrl* CreateSceneTree();

    wxTreeListCtrl* m_sceneTree;
};

#endif
