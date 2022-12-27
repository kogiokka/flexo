#ifndef ADD_DIALOG_H
#define ADD_DIALOG_H

#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>

class AddDialog : public wxDialog
{
public:
    AddDialog(wxWindow* parent, wxString const& title, int numRows);
    wxTextCtrl* AddInputInteger(wxString const& label, int value);
    wxTextCtrl* AddInputFloat(wxString const& label, float value);

protected:
    void AppendControl(wxString const& labelText, wxControl* control);
    wxTextCtrl* CreateLabel(wxString const& text);

    wxFlexGridSizer* m_grid;
    wxSizerFlags m_labelFlags;
    wxSizerFlags m_ctrlFlags;
    wxFont m_widgetFont;
    wxFont m_widgetLabelFont;

    wxDECLARE_NO_COPY_CLASS(AddDialog);
};

#endif
