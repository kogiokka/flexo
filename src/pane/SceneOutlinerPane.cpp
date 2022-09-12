#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "World.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "pane/SceneOutlinerPane.hpp"

// Register factory: SceneOutlinerPane
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        return new SceneOutlinerPane(mainPage, project);
    }
};

SceneOutlinerPane& SceneOutlinerPane::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<SceneOutlinerPane>(factoryKey);
}

SceneOutlinerPane const& SceneOutlinerPane::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

SceneOutlinerPane::SceneOutlinerPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    m_sceneTree = CreateSceneTree();
    GetSizer()->Add(m_sceneTree, wxSizerFlags(5).Expand());

    Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent&) {
        m_sceneTree->DeleteAllItems();
        // Each drawable has only one task currently
        for (auto const& task : Renderer::Get(m_project).GetTasks()) {
            auto drawable = task.mDrawable;
            auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), drawable->GetName());
            m_sceneTree->CheckItem(item, drawable->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
        }
    });

    m_sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [this](wxTreeListEvent& event) {
        wxTreeListItem const item = event.GetItem();
        wxString const text = m_sceneTree->GetItemText(item);
        for (auto const& task : Renderer::Get(m_project).GetTasks()) {
            auto drawable = task.mDrawable;
            if (text == drawable->GetName()) {
                switch (m_sceneTree->GetCheckedState(item)) {
                case wxCHK_CHECKED:
                    drawable->SetVisible(true);
                    break;
                case wxCHK_UNCHECKED:
                    drawable->SetVisible(false);
                    break;
                default:
                    break;
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
