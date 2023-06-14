#ifndef VIEWPORT_SETTINGS_DIALOG_H
#define VIEWPORT_SETTINGS_DIALOG_H

#include <wx/dialog.h>
#include <wx/sizer.h>

#include <pane/SceneViewportPane.hpp>

wxDECLARE_EVENT(EVT_VIEWPORT_SETTINGS_OVERLAY_GUIDES, wxCommandEvent);
wxDECLARE_EVENT(EVT_VIEWPORT_SETTINGS_BACKGROUND_DARK, wxCommandEvent);
wxDECLARE_EVENT(EVT_VIEWPORT_SETTINGS_BACKGROUND_LIGHT, wxCommandEvent);

class FlexoProject;

class ViewportSettingsDialog : public wxDialog
{
public:
    ViewportSettingsDialog(wxWindow* parent, SceneViewportPane::Settings settings, FlexoProject& project);

private:
    void OnGuidesCheckbox(wxCommandEvent& event);

    wxSizer* m_topLayout;
    FlexoProject& m_project;

    wxDECLARE_NO_COPY_CLASS(ViewportSettingsDialog);
};

#endif
