#ifndef VIEWPORT_DISPLAY_WIDGET
#define VIEWPORT_DISPLAY_WIDGET

#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>

#include "pane/ControlsGroup.hpp"

#define VIEWPORT_DISPLAY_LIST                                                                                          \
    X(ViewportDisplay_Solid, "Solid")                                                                                  \
    X(ViewportDisplay_Textured, "Textured")                                                                            \
    X(ViewportDisplay_Wire, "Wire")

#define X(type, name) type,
enum ViewportDisplays : unsigned int { VIEWPORT_DISPLAY_LIST };
#undef X

wxDECLARE_EVENT(EVT_VIEWPORT_DISPLAY_WIDGET_CHECK_WIREFRAME, wxCommandEvent);
wxDECLARE_EVENT(EVT_VIEWPORT_DISPLAY_WIDGET_SELECT_DISPLAY, wxCommandEvent);

class FlexoProject;

class ViewportDisplayWidget : public ControlsGroup
{
public:
    ViewportDisplayWidget(wxWindow* parent, FlexoProject& project);
    void SetWireframe(bool checked);
    void SetDisplay(unsigned int display);

private:
    void OnCheckWireframe(wxCommandEvent& event);
    void OnSelectDisplay(wxCommandEvent& event);

    wxCheckBox* m_wireframe;
    wxComboBox* m_displays;
    FlexoProject& m_project;
};

#endif
