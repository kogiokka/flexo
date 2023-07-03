#ifndef OUTLINER_PANE_H
#define OUTLINER_PANE_H

#include <memory>
#include <wx/clntdata.h>
#include <wx/event.h>
#include <wx/treelist.h>

#include "Attachable.hpp"
#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_OUTLINER_ADD_OBJECT, wxCommandEvent);
wxDECLARE_EVENT(EVT_OUTLINER_DELETE_OBJECT, wxCommandEvent);

class Object;

class OutlinerPane : public ControlsPaneBase, public AttachableBase
{
public:
    static OutlinerPane& Get(FlexoProject& project);
    static OutlinerPane const& Get(FlexoProject const& project);

    OutlinerPane(wxWindow* parent, FlexoProject& project);

private:
    void OnAddObject(wxCommandEvent& event);
    void OnItemContextMenu(wxTreeListEvent& event);
    void OnItemChecked(wxTreeListEvent& event);
    void OnSelectionChanged(wxTreeListEvent& event);

    wxTreeListCtrl* m_treelist;
};

class OutlinerItemData : public wxClientData
{
public:
    OutlinerItemData(std::shared_ptr<Object> const& object)
        : m_object(object)
    {
    }

    std::weak_ptr<Object> const& GetObject() const
    {
        return m_object;
    }

private:
    std::weak_ptr<Object> m_object;
};

#endif
