#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <wx/event.h>

#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_OBJECT_CHANGED, wxCommandEvent);

class WatermarkingProject;

class PropertiesPane : public ControlsPaneBase
{
public:
    PropertiesPane(wxWindow* parent, WatermarkingProject& project);

private:
};

#endif
