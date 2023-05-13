#include "pane/GuidesPropertiesPane.hpp"
#include "Project.hpp"

GuidesPropertiesPane::GuidesPropertiesPane(wxWindow* parent, FlexoProject& project)
    : ObjectPropertiesPane(parent, project)
{
}

GuidesPropertiesPane::~GuidesPropertiesPane()
{
}

void GuidesPropertiesPane::BindObject(std::shared_ptr<Object> obj)
{
    m_obj = obj;
}
