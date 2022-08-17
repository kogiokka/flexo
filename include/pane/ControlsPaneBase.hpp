#ifndef PANE_H
#define PANE_H

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
    wxFlexGridSizer* CreateGroup(int rows);
    wxTextCtrl* CreateLabel(wxString const& text);
    void AppendControl(wxFlexGridSizer* group, wxString const& labelText, wxControl* control);
    void AppendSizer(wxFlexGridSizer* group, wxString const& labelText, wxSizer* sizer);

    WatermarkingProject& m_project;
};

#endif
