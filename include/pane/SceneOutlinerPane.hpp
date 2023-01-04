#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include <wx/event.h>
#include <wx/treelist.h>

#include "Attachable.hpp"
#include "Mesh.hpp"
#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_OUTLINER_ADD_OBJECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_OUTLINER_DELETE_OBJECT, wxCommandEvent);

class SceneOutlinerPane : public ControlsPaneBase, public AttachableBase
{
public:
    static SceneOutlinerPane& Get(FlexoProject& project);
    static SceneOutlinerPane const& Get(FlexoProject const& project);

    SceneOutlinerPane(wxWindow* parent, FlexoProject& project);

private:
    wxTreeListCtrl* CreateSceneTree();
    void OnAddObject(wxCommandEvent& event);

    wxTreeListCtrl* m_sceneTree;
};

#endif
