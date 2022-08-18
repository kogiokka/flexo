#ifndef CONTROLS_PANE_H
#define CONTROLS_PANE_H

#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class WatermarkingProject;

class ControlsPaneBase : public wxScrolledWindow
{
    wxSizerFlags m_labelFlags;
    wxSizerFlags m_ctrlFlags;

public:
    ControlsPaneBase(wxWindow* parent, WatermarkingProject& project);

protected:
    wxWindow* CreateControlGroup(wxString const& title, int rows);
    void AppendControl(wxWindow* parent, wxString const& labelText, wxControl* control);
    void AppendSizer(wxWindow* parent, wxString const& labelText, wxSizer* sizer);

    WatermarkingProject& m_project;

private:
    wxTextCtrl* CreateLabel(wxWindow* parent, wxString const& text);
};

#endif
