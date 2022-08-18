#ifndef CONTROLS_PANE_BASE_H
#define CONTROLS_PANE_BASE_H

#include <wx/scrolwin.h>
#include <wx/string.h>
#include <wx/window.h>

#include "pane/ControlsGroup.hpp"

class WatermarkingProject;

class ControlsPaneBase : public wxScrolledWindow
{
public:
    ControlsPaneBase(wxWindow* parent, WatermarkingProject& project);

protected:
    ControlsGroup* AddGroup(wxString const& title, int numRows);

    WatermarkingProject& m_project;
};

#endif
