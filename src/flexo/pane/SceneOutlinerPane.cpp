#include <wx/checkbox.h>
#include <wx/clntdata.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "gfx/DrawableBase.hpp"
#include "gfx/Renderer.hpp"
#include "log/Logger.h"
#include "object/ObjectList.hpp"
#include "pane/PropertiesPane.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

wxDEFINE_EVENT(EVT_OUTLINER_ADD_OBJECT, wxCommandEvent);
wxDEFINE_EVENT(EVT_OUTLINER_DELETE_OBJECT, wxCommandEvent);

// Register factory: SceneOutlinerPane
static FlexoProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        return new SceneOutlinerPane(mainPage, project);
    }
};

SceneOutlinerPane& SceneOutlinerPane::Get(FlexoProject& project)
{
    return project.AttachedWindows::Get<SceneOutlinerPane>(factoryKey);
}

SceneOutlinerPane const& SceneOutlinerPane::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

SceneOutlinerPane::SceneOutlinerPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
{
    m_sceneTree = CreateSceneTree();
    GetSizer()->Add(m_sceneTree, wxSizerFlags(5).Expand());

    m_project.Bind(EVT_OUTLINER_ADD_OBJECT, &SceneOutlinerPane::OnAddObject, this);
    m_project.Bind(EVT_OUTLINER_DELETE_OBJECT, [this](wxCommandEvent&) {
        m_sceneTree->DeleteAllItems();
        for (auto const& drawable : ObjectList::Get(m_project)) {
            auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), drawable->GetID());
            m_sceneTree->CheckItem(item, drawable->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
        }
    });

    m_sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CONTEXT_MENU, [this](wxTreeListEvent& event) {
        enum { ID_SelectTarget, ID_Delete };
        wxMenu menu;
        menu.Append(ID_SelectTarget, "&Select as target");
        menu.Append(ID_Delete, "&Delete");
        switch (m_sceneTree->GetPopupMenuSelectionFromUser(menu)) {
        case ID_SelectTarget: {
            auto item = event.GetItem();
            auto id = m_sceneTree->GetItemText(item);
            for (auto const& obj : ObjectList::Get(m_project)) {
                if (obj->GetID() == id) {
                    auto const& pos = obj->GetPositions();
                    m_project.theModel = obj;
                    m_project.theDataset = std::make_shared<Dataset<3>>(pos);
                    log_info("Dataset count: %lu", pos.size());
                    wxCommandEvent event(EVT_SOM_PANE_TARGET_CHANGED);
                    event.SetString(id);
                    m_project.ProcessEvent(event);
                    break;
                }
            }
        } break;
        case ID_Delete: {
            auto item = event.GetItem();

            wxCommandEvent event(EVT_OBJECTLIST_DELETE_OBJECT);
            event.SetString(m_sceneTree->GetItemText(item).ToStdString());
            m_project.ProcessEvent(event);

            m_sceneTree->DeleteItem(item);
        } break;
        }
    });

    m_sceneTree->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, [this](wxTreeListEvent& event) {
        wxTreeListItem const item = event.GetItem();
        wxString const text = m_sceneTree->GetItemText(item);
        for (auto const& obj : ObjectList::Get(m_project)) {
            if (text == obj->GetID()) {
                switch (m_sceneTree->GetCheckedState(item)) {
                case wxCHK_CHECKED:
                    obj->SetVisible(true);
                    break;
                case wxCHK_UNCHECKED:
                    obj->SetVisible(false);
                    break;
                default:
                    break;
                }
            }
        }
    });

    m_sceneTree->Bind(wxEVT_TREELIST_SELECTION_CHANGED, [this](wxTreeListEvent& event) {
        wxTreeListItem const item = event.GetItem();
        wxString const id = m_sceneTree->GetItemText(item);
        wxCommandEvent evt(EVT_PROPERTIES_PANE_OBJECT_CHANGED);
        evt.SetString(id);
        m_project.ProcessEvent(evt);
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

void SceneOutlinerPane::OnAddObject(wxCommandEvent& event)
{
    for (auto const& obj : ObjectList::Get(m_project)) {
        if (event.GetString() == obj->GetID()) {
            auto item = m_sceneTree->AppendItem(m_sceneTree->GetRootItem(), obj->GetID());
            m_sceneTree->CheckItem(item, obj->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
            break;
        }
    }
}
