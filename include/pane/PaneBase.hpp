#ifndef PANE_H
#define PANE_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/window.h>

class WatermarkingProject;

class PaneBase : public wxScrolledWindow
{
public:
    PaneBase(wxWindow* parent, WatermarkingProject& project);

protected:
    WatermarkingProject& m_project;
};

#endif
