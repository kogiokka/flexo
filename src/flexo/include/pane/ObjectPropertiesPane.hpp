#ifndef OBJECT_PROPERTIES_PANE_H
#define OBJECT_PROPERTIES_PANE_H

#include <memory>

#include <wx/panel.h>

class Object;
class FlexoProject;

class ObjectPropertiesPane : public wxPanel
{
public:
    ObjectPropertiesPane(wxWindow* parent, FlexoProject& project)
        : wxPanel(parent, wxID_ANY)
        , m_project(project)
    {
    }

    virtual ~ObjectPropertiesPane()
    {
    }

    virtual void BindObject(std::shared_ptr<Object>) = 0;

protected:
    std::shared_ptr<Object> m_obj;
    FlexoProject& m_project;
};

#endif
