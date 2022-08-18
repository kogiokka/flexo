#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/treelist.h>

#include "World.hpp"
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
    GetSizer()->Add(sceneTree, wxSizerFlags(5).Expand());

    auto* panel = CreateControlGroup("Properties", 1);
    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(panel, wxID_ANY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS);
    AppendControl(panel, "Model Transparency (%)", m_slider);

    sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [sceneTree](wxTreeListEvent& event) {
        auto* data = sceneTree->GetItemData(event.GetItem());
        RenderFlag opt = static_cast<IsShownFlag*>(data)->GetFlag();
        world.renderFlags ^= opt;
    });

    m_slider->Bind(wxEVT_SLIDER, &SceneOutlinerPane::OnSliderTransparency, this);
}

void SceneOutlinerPane::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}
