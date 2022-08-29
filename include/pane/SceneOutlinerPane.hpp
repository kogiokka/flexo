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

    void SubmitLattice(Mesh const& vertexMesh, Mesh const& perVertexData, Mesh const& latticeMesh,
                       std::vector<unsigned int> const& indices);
    void SubmitPolygon(Mesh const& mesh);
    void SubmitVolume(Mesh const& instanceMesh, Mesh const& perInstanceData);

private:
    wxTreeListCtrl* CreateSceneTree();

    wxTreeListCtrl* m_sceneTree;
};

#endif
