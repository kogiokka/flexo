#ifndef SCENE_OUTLINER_PANE_H
#define SCENE_OUTLINER_PANE_H

#include <string>

#include <wx/treelist.h>

#include "pane/ControlsPaneBase.hpp"

class SceneOutlinerPane : public ControlsPaneBase
{
public:
    SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project);

private:
    wxTreeListCtrl* CreateSceneTree();
    void OnUpdateUI(wxUpdateUIEvent& event);
    wxString CreateItemText(std::string const& name, unsigned int id) const;

    wxTreeListCtrl* m_sceneTree;
};

#endif
