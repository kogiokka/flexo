#ifndef OBJECT_PROPERTIES_PANE_H
#define OBJECT_PROPERTIES_PANE_H

#include <memory>

class Object;

class ObjectPropertiesPane
{
public:
    virtual ~ObjectPropertiesPane() {};
    virtual void BindObject(std::shared_ptr<Object>) = 0;

protected:
    std::shared_ptr<Object> m_obj;
};

#endif
