#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/treelist.h>

#include "World.hpp"
#include "pane/ControlsGroup.hpp"
#include "pane/SceneOutlinerPane.hpp"

class IsShownFlag : public wxClientData
{
    RenderFlag mFlag;

public:
    IsShownFlag(RenderFlag flag)
        : mFlag(flag) {};
    virtual ~IsShownFlag() override
    {
    }
    RenderFlag GetFlag() const
    {
        return mFlag;
    }
};

SceneOutlinerPane::SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto sceneTree
        = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_CHECKBOX | wxTL_NO_HEADER);
    sceneTree->AppendColumn("");

    auto latticeNode = sceneTree->AppendItem(sceneTree->GetRootItem(), "Lattice", wxTreeListCtrl::NO_IMAGE,
                                             wxTreeListCtrl::NO_IMAGE, new IsShownFlag(RenderFlag_DrawLattice));
    auto item1 = sceneTree->AppendItem(latticeNode, "Vertex", wxTreeListCtrl::NO_IMAGE, wxTreeListCtrl::NO_IMAGE,
                                       new IsShownFlag(RenderFlag_DrawLatticeVertex));
    auto item2 = sceneTree->AppendItem(latticeNode, "Edge", wxTreeListCtrl::NO_IMAGE, wxTreeListCtrl::NO_IMAGE,
                                       new IsShownFlag(RenderFlag_DrawLatticeEdge));
    auto item3 = sceneTree->AppendItem(latticeNode, "Face", wxTreeListCtrl::NO_IMAGE, wxTreeListCtrl::NO_IMAGE,
                                       new IsShownFlag(RenderFlag_DrawLatticeFace));

    auto item4 = sceneTree->AppendItem(sceneTree->GetRootItem(), "Model", wxTreeListCtrl::NO_IMAGE,
                                       wxTreeListCtrl::NO_IMAGE, new IsShownFlag(RenderFlag_DrawModel));
    auto item5 = sceneTree->AppendItem(sceneTree->GetRootItem(), "Light", wxTreeListCtrl::NO_IMAGE,
                                       wxTreeListCtrl::NO_IMAGE, new IsShownFlag(RenderFlag_DrawLightSource));
    sceneTree->CheckItem(item1, (world.renderFlags & RenderFlag_DrawLatticeVertex) ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    sceneTree->CheckItem(item2, (world.renderFlags & RenderFlag_DrawLatticeEdge) ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    sceneTree->CheckItem(item3, (world.renderFlags & RenderFlag_DrawLatticeFace) ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    sceneTree->CheckItem(item4, (world.renderFlags & RenderFlag_DrawModel) ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    sceneTree->CheckItem(item5, (world.renderFlags & RenderFlag_DrawLightSource) ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    sceneTree->CheckItem(
        latticeNode,
        (world.renderFlags & (RenderFlag_DrawLatticeVertex | RenderFlag_DrawLatticeEdge | RenderFlag_DrawLatticeFace))
            ? wxCHK_CHECKED
            : wxCHK_UNCHECKED);

    auto* group = new ControlsGroup(this, "Properties", 1);
    m_slider = group->AddSliderFloat("Model Transparency (%)", (100.0f - world.modelColorAlpha * 100.0f), 0.0f, 100.0f);

    GetSizer()->Add(sceneTree, wxSizerFlags(5).Expand());
    GetSizer()->Add(group, wxSizerFlags().Expand());

    sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [sceneTree](wxTreeListEvent& event) {
        auto* data = sceneTree->GetItemData(event.GetItem());
        RenderFlag opt = static_cast<IsShownFlag*>(data)->GetFlag();
        world.renderFlags ^= opt;
    });

    group->Bind(EVT_SLIDER_FLOAT, [](SliderFloatEvent& event) {
        float const value = event.GetValue();
        world.modelColorAlpha = (100.0f - value) / 100.0f;
    });
}
