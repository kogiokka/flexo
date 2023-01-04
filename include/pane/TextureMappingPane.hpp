#ifndef TEXTURE_MAPPING_PANE_H
#define TEXTURE_MAPPING_PANE_H

#include <wx/event.h>

#include "pane/ControlsPaneBase.hpp"

class TextureMappingPane : public ControlsPaneBase
{
public:
    TextureMappingPane(wxWindow* parent, WatermarkingProject& project);
};

#endif
