#ifndef GUIDES_PROPERTIES_PANE_H
#define GUIDES_PROPERTIES_PANE_H

#include <wx/window.h>

#include "pane/ObjectPropertiesPane.hpp"

class FlexoProject;

class GuidesPropertiesPane : public ObjectPropertiesPane
{
public:
    GuidesPropertiesPane(wxWindow* parent, FlexoProject& project);
    virtual ~GuidesPropertiesPane() override;
    virtual void BindObject(std::shared_ptr<Object> obj) override;
};

#endif
