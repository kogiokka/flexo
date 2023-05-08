#ifndef MESH_OBJECT_PROPERTIES_PANE_H
#define MESH_OBJECT_PROPERTIES_PANE_H

#include <memory>

#include "object/Object.hpp"
#include "pane/ObjectPropertiesPane.hpp"
#include "pane/TransfromWidget.hpp"
#include "pane/ViewportDisplayWidget.hpp"

class FlexoProject;
class PropertiesPane;

class MeshObjectPropertiesPane : public ObjectPropertiesPane
{
public:
    MeshObjectPropertiesPane(PropertiesPane& m_parent, FlexoProject& project);
    virtual ~MeshObjectPropertiesPane();
    virtual void BindObject(std::shared_ptr<Object> obj) override;

private:
    void OnSelectDisplay(wxCommandEvent& event);
    void OnCheckWireframe(wxCommandEvent& event);
    void OnTransformLocation(Vec3Event& event);
    void OnTransformRotation(Vec3Event& event);
    void OnTransformScale(Vec3Event& event);
    void OnTransformApply(Vec3Event& event);

    bool m_hasWireframe;
    ViewportDisplayWidget* m_display;
    TransformWidget* m_transform;
    PropertiesPane& m_parent;
    FlexoProject& m_project;
};

#endif
