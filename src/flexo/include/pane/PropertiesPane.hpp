#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <memory>

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/weakref.h>

#include "event/ObjectSelectEvent.hpp"
#include "pane/ControlsPaneBase.hpp"
#include "pane/ObjectPropertiesPane.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

class FlexoProject;

class PropertiesPane : public wxScrolledWindow
{
public:
    PropertiesPane(wxWindow* parent, FlexoProject& project);

private:
    void OnObjectSelected(ObjectSelectEvent& event);

    wxWeakRef<ObjectPropertiesPane> m_props;
    wxSizer* m_layout;
    FlexoProject& m_project;
};

#endif
