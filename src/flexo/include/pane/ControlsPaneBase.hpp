#ifndef CONTROLS_PANE_BASE_H
#define CONTROLS_PANE_BASE_H

#include <wx/scrolwin.h>
#include <wx/string.h>
#include <wx/window.h>

#include "pane/ControlsGroup.hpp"

class FlexoProject;

class ControlsPaneBase : public wxScrolledWindow
{
public:
    ControlsPaneBase(wxWindow* parent, FlexoProject& project);

protected:
    ControlsGroup* AddGroup(wxString const& title, int numRows);

    FlexoProject& m_project;
};

#endif
