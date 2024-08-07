#ifndef CONTROLS_GROUP_H
#define CONTROLS_GROUP_H

#include <wx/bmpcbox.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/collpane.h>
#include <wx/control.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/window.h>

class ControlsGroup : public wxCollapsiblePane
{
    wxFlexGridSizer* m_grid;
    wxSizerFlags m_labelFlags;
    wxSizerFlags m_ctrlFlags;
    wxFont m_widgetFont;
    wxFont m_widgetLabelFont;

public:
    ControlsGroup(wxWindow* parent, wxString const& title, int numRows);
    wxButton* AddButton(wxString const& label);
    wxCheckBox* AddCheckBoxWithHeading(wxString const& heading, wxString const& label, bool value);
    wxCheckBox* AddCheckBox(wxString const& label, bool value);
    wxRadioButton* AddRadioButtonWithHeading(wxString const& heading, wxString const& label, bool value);
    wxRadioButton* AddRadioButton(wxString const& label, bool value);
    wxTextCtrl* AddInputText(wxString const& label, wxString const& value = wxEmptyString);
    wxTextCtrl* AddReadOnlyText(wxString const& label, wxString const& value = wxEmptyString);
    wxSlider* AddSliderFloat(wxString const& label, float value, float minValue, float maxValue);
    wxComboBox* AddComboBox(wxString const& label);
    wxBitmapComboBox* AddBitmapComboBox(wxString const& label);

private:
    void AppendControl(wxString const& labelText, wxControl* control);
    void AppendSizer(wxString const& labelText, wxSizer* sizer);
    wxTextCtrl* CreateLabel(wxString const& text);
};

#endif
