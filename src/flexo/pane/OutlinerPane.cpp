#include <wx/menu.h>
#include <wx/sizer.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SceneController.hpp"
#include "event/ObjectSelectEvent.hpp"
#include "log/Logger.h"
#include "object/Object.hpp"
#include "pane/OutlinerPane.hpp"
#include "pane/PropertiesPane.hpp"

wxDEFINE_EVENT(EVT_OUTLINER_ADD_OBJECT, wxCommandEvent);

// Register factory: OutlinerPane
static FlexoProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        return new OutlinerPane(mainPage, project);
    }
};

OutlinerPane& OutlinerPane::Get(FlexoProject& project)
{
    return project.AttachedWindows::Get<OutlinerPane>(factoryKey);
}

OutlinerPane const& OutlinerPane::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

OutlinerPane::OutlinerPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
{
    wxFont font = GetFont();
    font.SetPointSize(font.GetPointSize() - 1);

    m_treelist = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_CHECKBOX | wxTL_NO_HEADER);
    m_treelist->AppendColumn("");
    m_treelist->SetFont(font);
    GetSizer()->Add(m_treelist, wxSizerFlags(5).Expand());

    m_project.Bind(EVT_OUTLINER_ADD_OBJECT, &OutlinerPane::OnAddObject, this);
    m_treelist->Bind(wxEVT_TREELIST_ITEM_CONTEXT_MENU, &OutlinerPane::OnItemContextMenu, this);
    m_treelist->Bind(wxEVT_TREELIST_ITEM_CHECKED, &OutlinerPane::OnItemChecked, this);
    m_treelist->Bind(wxEVT_TREELIST_SELECTION_CHANGED, &OutlinerPane::OnSelectionChanged, this);
}

void OutlinerPane::OnAddObject(wxCommandEvent& event)
{
    OutlinerItemData* data = static_cast<OutlinerItemData*>(event.GetClientData());

    if (auto obj = data->GetObject().lock()) {
        auto item = m_treelist->AppendItem(m_treelist->GetRootItem(), obj->GetID());
        m_treelist->SetItemData(item, data);
        m_treelist->CheckItem(item, obj->IsVisible() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    }
}

void OutlinerPane::OnItemContextMenu(wxTreeListEvent& event)
{
    enum { ID_Delete };

    wxMenu menu;
    menu.Append(ID_Delete, "&Delete");

    switch (m_treelist->GetPopupMenuSelectionFromUser(menu)) {
    case ID_Delete: {
        auto item = event.GetItem();
        OutlinerItemData const* data = dynamic_cast<OutlinerItemData*>(m_treelist->GetItemData(item));

        wxCommandEvent event(EVT_DELETE_OBJECT);
        event.SetString(data->GetObject().lock()->GetID());
        m_project.ProcessEvent(event);

        m_treelist->DeleteItem(item);
    } break;
    }
}

void OutlinerPane::OnItemChecked(wxTreeListEvent& event)
{
    wxTreeListItem const item = event.GetItem();
    OutlinerItemData const* data = dynamic_cast<OutlinerItemData*>(m_treelist->GetItemData(item));

    auto state = m_treelist->GetCheckedState(item);

    if (auto obj = data->GetObject().lock()) {
        switch (state) {
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

void OutlinerPane::OnSelectionChanged(wxTreeListEvent& event)
{
    OutlinerItemData const* data = dynamic_cast<OutlinerItemData*>(m_treelist->GetItemData(event.GetItem()));

    // when treelist is empty
    if (!data) {
        return;
    }

    if (auto obj = data->GetObject().lock()) {
        ObjectSelectEvent evt(EVT_PROPERTIES_PANE_OBJECT_SELECTED, this->GetId(), obj->GetID(), obj->GetType());
        m_project.ProcessEvent(evt);
    }
}
