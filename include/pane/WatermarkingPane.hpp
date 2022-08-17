#ifndef WATERMARK_PANEL
#define WATERMARK_PANEL

#include <wx/event.h>

#include "pane/ControlsPaneBase.hpp"

class WatermarkingPane : public ControlsPaneBase
{
public:
    WatermarkingPane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnWatermark(wxCommandEvent& event);
};

#endif
