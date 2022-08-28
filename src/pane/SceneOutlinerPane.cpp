#include <iomanip>
#include <sstream>

#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "Renderer.hpp"
#include "World.hpp"
#include "pane/SceneOutlinerPane.hpp"

SceneOutlinerPane::SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    m_sceneTree = CreateSceneTree();
    GetSizer()->Add(m_sceneTree, wxSizerFlags(5).Expand());

    Bind(wxEVT_UPDATE_UI, &SceneOutlinerPane::OnUpdateUI, this);

    m_sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [this](wxTreeListEvent& event) {
        wxTreeListItem const item = event.GetItem();
        wxString const text = m_sceneTree->GetItemText(item);
        for (auto& [name, objs] : Renderer::Get(m_project).GetDrawables()) {
            for (unsigned int i = 0; i < objs.size(); i++) {
                if (text == CreateItemText(name, i)) {
                    switch (m_sceneTree->GetCheckedState(item)) {
                    case wxCHK_CHECKED:
                        objs[i]->SetVisible(true);
                        break;
                    case wxCHK_UNCHECKED:
                        objs[i]->SetVisible(false);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    });

    auto* group = AddGroup("Properties", 1);
    auto* slider
        = group->AddSliderFloat("Model Transparency (%)", (100.0f - world.modelColorAlpha * 100.0f), 0.0f, 100.0f);
    slider->Bind(EVT_SLIDER_FLOAT, [](SliderFloatEvent& event) {
        float const value = event.GetValue();
        world.modelColorAlpha = (100.0f - value) / 100.0f;
    });
}

wxTreeListCtrl* SceneOutlinerPane::CreateSceneTree()
{
    auto* sceneTree
        = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_CHECKBOX | wxTL_NO_HEADER);
    sceneTree->AppendColumn("");
    wxFont font = GetFont();
    font.SetPointSize(font.GetPointSize() - 1);
    sceneTree->SetFont(font);

    return sceneTree;
}

void SceneOutlinerPane::OnUpdateUI(wxUpdateUIEvent&)
{
    m_sceneTree->DeleteAllItems();
    for (auto const& [name, objs] : Renderer::Get(m_project).GetDrawables()) {
        for (unsigned int i = 0; i < objs.size(); i++) {
            auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), CreateItemText(name, i));
            m_sceneTree->CheckItem(item, objs[i]->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
        }
    }
}
