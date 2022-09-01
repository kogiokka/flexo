#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include <wx/treelist.h>

#include "Attachable.hpp"
#include "Mesh.hpp"
#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase, public AttachableBase
{
public:
    static SceneOutlinerPane& Get(WatermarkingProject& project);
    static SceneOutlinerPane const& Get(WatermarkingProject const& project);

    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);
private:
    wxTreeListCtrl* CreateSceneTree();

    wxTreeListCtrl* m_sceneTree;
};

#endif
