#ifndef CONTROLS_GROUP_H
#define CONTROLS_GROUP_H

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/collpane.h>
#include <wx/control.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class SliderFloatEvent : public wxEvent
{
public:
    SliderFloatEvent(wxEventType eventType, int winid, float value)
        : wxEvent(winid, eventType)
        , m_value(value)
    {
    }
    float GetValue() const
    {
        return m_value;
    }
    virtual wxEvent* Clone() const
    {
        return new SliderFloatEvent(*this);
    }

private:
    float m_value;
};

wxDECLARE_EVENT(EVT_SLIDER_FLOAT, SliderFloatEvent);
typedef void (wxEvtHandler::*SliderFloatEventFunction)(SliderFloatEvent&);
#define SliderFloatEventHandler(func) (&func)

class ControlsGroup : public wxCollapsiblePane
{
    wxFlexGridSizer* m_grid;
    wxSizerFlags m_labelFlags;
    wxSizerFlags m_ctrlFlags;

public:
    ControlsGroup(wxWindow* parent, wxString const& title, int numRows);
    wxButton* AddButton(wxString const& label);
    wxCheckBox* AddCheckBox(wxString const& label, bool value);
    wxTextCtrl* AddInputText(wxString const& label, wxString const& value = wxEmptyString);
    wxTextCtrl* AddReadOnlyText(wxString const& label, wxString const& value = wxEmptyString);
    wxSlider* AddSliderFloat(wxString const& label, float value, float minValue, float maxValue);

private:
    void AppendControl(wxString const& labelText, wxControl* control);
    void AppendSizer(wxString const& labelText, wxSizer* sizer);
    wxTextCtrl* CreateLabel(wxString const& text);
};

#endif
