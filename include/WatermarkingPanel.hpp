#ifndef WATERMARK_PANEL
#define WATERMARK_PANEL

#include <wx/event.h>

#include "pane/PaneBase.hpp"

class WatermarkingPanel : public PaneBase
{
public:
    WatermarkingPanel(wxWindow* parent, WatermarkingProject& project);

private:
    void OnWatermark(wxCommandEvent& event);
};

#endif
