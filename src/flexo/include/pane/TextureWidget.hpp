#ifndef TEXTURE_WIDGET_H
#define TEXTURE_WIDGET_H

#include <wx/button.h>
#include <wx/event.h>
#include <wx/textctrl.h>

#include "pane/ControlsGroup.hpp"

wxDECLARE_EVENT(EVT_TEXTURE_WIDGET_OPEN_IMAGE, wxCommandEvent);

class FlexoProject;

class TextureWidget : public ControlsGroup
{
public:
    TextureWidget(wxWindow* parent, FlexoProject& project);

private:
    void OnOpenImage(wxCommandEvent& event);

    wxTextCtrl* m_text;
    wxButton* m_open;
    FlexoProject& m_project;
};

#endif
