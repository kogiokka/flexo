#ifndef WATERMARK_PANEL
#define WATERMARK_PANEL

#include <wx/event.h>

#include "pane/PaneBase.hpp"

class WatermarkingPane : public PaneBase
{
public:
    WatermarkingPane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnWatermark(wxCommandEvent& event);
};

#endif
