#ifndef WATERMARKING_PANE_H
#define WATERMARKING_PANE_H

#include <wx/event.h>

#include "pane/ControlsPaneBase.hpp"

class WatermarkingPane : public ControlsPaneBase
{
public:
    WatermarkingPane(wxWindow* parent, WatermarkingProject& project);
};

#endif
