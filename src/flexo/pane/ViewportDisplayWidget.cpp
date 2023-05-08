#include <string>

#include "Project.hpp"
#include "pane/ViewportDisplayWidget.hpp"

wxDEFINE_EVENT(EVT_VIEWPORT_DISPLAY_WIDGET_CHECK_WIREFRAME, wxCommandEvent);
wxDEFINE_EVENT(EVT_VIEWPORT_DISPLAY_WIDGET_SELECT_DISPLAY, wxCommandEvent);

#define X(type, name) name,
static std::string ViewportDisplayNames[] = { VIEWPORT_DISPLAY_LIST };
#undef X

ViewportDisplayWidget::ViewportDisplayWidget(wxWindow* parent, FlexoProject& project)
    : ControlsGroup(parent, "Viewport Display", 2)
    , m_wireframe(nullptr)
    , m_displays(nullptr)
    , m_project(project)
{
    m_wireframe = AddCheckBoxWithHeading("Show", "Wireframe", false);
    m_displays = AddComboBox("Display As");

    for (auto const& name : ViewportDisplayNames) {
        m_displays->Append(name);
    }
    m_displays->SetSelection(ViewportDisplay_Solid);

    m_wireframe->Bind(wxEVT_CHECKBOX, &ViewportDisplayWidget::OnCheckWireframe, this);
    m_displays->Bind(wxEVT_COMBOBOX, &ViewportDisplayWidget::OnSelectDisplay, this);
}

void ViewportDisplayWidget::SetWireframe(bool checked)
{
    m_wireframe->SetValue(checked);
}

void ViewportDisplayWidget::SetDisplay(unsigned int display)
{
    m_displays->SetSelection(display);
}

void ViewportDisplayWidget::OnCheckWireframe(wxCommandEvent& event)
{
    wxCommandEvent signal(EVT_VIEWPORT_DISPLAY_WIDGET_CHECK_WIREFRAME);
    signal.SetInt(event.GetInt());
    m_project.ProcessEvent(signal);
}

void ViewportDisplayWidget::OnSelectDisplay(wxCommandEvent& event)
{
    wxCommandEvent signal(EVT_VIEWPORT_DISPLAY_WIDGET_SELECT_DISPLAY);
    signal.SetInt(event.GetSelection());
    m_project.ProcessEvent(signal);
}
