#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include <wx/event.h>
#include <wx/treelist.h>

#include "Attachable.hpp"
#include "Mesh.hpp"
#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_OUTLINER_ADD_OBJECT, wxCommandEvent);

class SceneOutlinerPane : public ControlsPaneBase, public AttachableBase
{
public:
    static SceneOutlinerPane& Get(WatermarkingProject& project);
    static SceneOutlinerPane const& Get(WatermarkingProject const& project);

    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);

private:
    wxTreeListCtrl* CreateSceneTree();
    void OnAddObject(wxCommandEvent& event);

    wxTreeListCtrl* m_sceneTree;
};

#endif
