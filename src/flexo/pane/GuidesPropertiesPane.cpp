#include "pane/GuidesPropertiesPane.hpp"

GuidesPropertiesPane::~GuidesPropertiesPane()
{
}

void GuidesPropertiesPane::BindObject(std::shared_ptr<Object> obj)
{
    m_obj = obj;
}
