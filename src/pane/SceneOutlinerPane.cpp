#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "World.hpp"
#include "gfx/DrawList.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "util/Logger.h"

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

    m_project.Bind(EVT_ADD_OBJECT, [this](wxCommandEvent& event) {
        for (auto const& drawable : DrawList::Get(m_project)) {
            if (event.GetString() == drawable->GetID()) {
                auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), drawable->GetID());
                m_sceneTree->CheckItem(item, drawable->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
                break;
            }
        }
    });

    m_project.Bind(EVT_REMOVE_OBJECT, [this](wxCommandEvent&) {
        m_sceneTree->DeleteAllItems();
        for (auto const& drawable : DrawList::Get(m_project)) {
            auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), drawable->GetID());
            m_sceneTree->CheckItem(item, drawable->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
        }
    });

    m_sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [this](wxTreeListEvent& event) {
        wxTreeListItem const item = event.GetItem();
        wxString const text = m_sceneTree->GetItemText(item);
        for (auto const& drawable : DrawList::Get(m_project)) {
            if (text == drawable->GetID()) {
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
