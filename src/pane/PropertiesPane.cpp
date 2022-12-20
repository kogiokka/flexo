#include "pane/PropertiesPane.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_OBJECT_CHANGED, wxCommandEvent);

PropertiesPane::PropertiesPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* group = AddGroup("Viewport Display", 1);
    group->AddCheckBoxWithHeading("Show", "Wireframe", false);
}
