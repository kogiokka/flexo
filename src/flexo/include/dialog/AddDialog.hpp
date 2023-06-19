#ifndef ADD_DIALOG_H
#define ADD_DIALOG_H

#include "log/Logger.h"
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/msgdlg.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

#include "SceneController.hpp"

class AddDialog : public wxDialog
{
public:
    AddDialog(wxWindow* parent, wxString const& title, int numRows);
    wxTextCtrl* AddInputInteger(wxString const& label, int value);
    wxTextCtrl* AddInputFloat(wxString const& label, float value);
    wxCheckBox* AddCheckBoxWithHeading(wxString const& heading, wxString const& label, bool value);
    wxCheckBox* AddCheckBox(wxString const& label, bool value);
    wxRadioButton* AddRadioButtonWithHeading(wxString const& heading, wxString const& label, bool value);
    wxRadioButton* AddRadioButton(wxString const& label, bool value);

protected:
    void AppendControl(wxString const& labelText, wxControl* control);
    wxTextCtrl* CreateLabel(wxString const& text);

    wxSizer* m_topSizer;
    wxFlexGridSizer* m_grid;
    wxSizerFlags m_labelFlags;
    wxSizerFlags m_ctrlFlags;
    wxFont m_widgetFont;
    wxFont m_widgetLabelFont;
    wxSize m_defaultSize;

    wxDECLARE_NO_COPY_CLASS(AddDialog);
};

#endif
