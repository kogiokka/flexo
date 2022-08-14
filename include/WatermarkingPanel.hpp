#ifndef WATERMARK_PANEL
#define WATERMARK_PANEL

#include <wx/event.h>
#include <wx/scrolwin.h>

enum {
    PANEL_WATERMARKING = wxID_HIGHEST + 30,
};

class WatermarkingProject;

class WatermarkingPanel : public wxScrolledWindow
{
public:
    WatermarkingPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                   WatermarkingProject& project);

private:
    void OnWatermark(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);

    WatermarkingProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
