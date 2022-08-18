#ifndef CONTROLS_PANE_BASE_H
#define CONTROLS_PANE_BASE_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class WatermarkingProject;

class ControlsPaneBase : public wxScrolledWindow
{
public:
    ControlsPaneBase(wxWindow* parent, WatermarkingProject& project);

protected:
    WatermarkingProject& m_project;
};

#endif
