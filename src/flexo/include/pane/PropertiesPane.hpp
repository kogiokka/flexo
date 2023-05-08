#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <memory>
#include <string>

#include <wx/event.h>

#include "event/ObjectSelectEvent.hpp"
#include "pane/ControlsPaneBase.hpp"
#include "pane/ObjectPropertiesPane.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

class FlexoProject;

class PropertiesPane : public ControlsPaneBase
{
public:
    PropertiesPane(wxWindow* parent, FlexoProject& project);

private:
    void OnObjectSelected(ObjectSelectEvent& event);

    std::shared_ptr<ObjectPropertiesPane> m_props;
    FlexoProject& m_project;
};

#endif
