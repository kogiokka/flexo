#ifndef GUIDES_PROPERTIES_PANE_H
#define GUIDES_PROPERTIES_PANE_H

#include "pane/ObjectPropertiesPane.hpp"

class GuidesPropertiesPane : public ObjectPropertiesPane
{
public:
    GuidesPropertiesPane() = default;
    virtual ~GuidesPropertiesPane() override;
    virtual void BindObject(std::shared_ptr<Object> obj) override;
};

#endif
