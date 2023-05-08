#include "pane/MeshObjectPropertiesPane.hpp"
#include "Project.hpp"
#include "pane/PropertiesPane.hpp"
#include "pane/SceneViewportPane.hpp"

MeshObjectPropertiesPane::MeshObjectPropertiesPane(PropertiesPane& m_parent, FlexoProject& project)
    : m_parent(m_parent)
    , m_project(project)
{
    m_project.Bind(EVT_TRANSFORM_WIDGET_LOCATION, &MeshObjectPropertiesPane::OnTransformLocation, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_ROTATION, &MeshObjectPropertiesPane::OnTransformRotation, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_SCALE, &MeshObjectPropertiesPane::OnTransformScale, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_APPLY, [this](wxCommandEvent&) {
        m_obj->ApplyTransform();
        m_obj->GenerateDrawables(SceneViewportPane::Get(m_project).GetGL());
    });

    m_project.Bind(EVT_VIEWPORT_DISPLAY_WIDGET_CHECK_WIREFRAME, &MeshObjectPropertiesPane::OnCheckWireframe, this);
    m_project.Bind(EVT_VIEWPORT_DISPLAY_WIDGET_SELECT_DISPLAY, &MeshObjectPropertiesPane::OnSelectDisplay, this);

    m_transform = new TransformWidget(&m_parent, m_project);
    m_display = new ViewportDisplayWidget(&m_parent, m_project);

    m_parent.AddGroup(m_transform);
    m_parent.AddGroup(m_display);
}

MeshObjectPropertiesPane::~MeshObjectPropertiesPane()
{
    delete m_transform;
    delete m_display;
}

void MeshObjectPropertiesPane::BindObject(std::shared_ptr<Object> obj)
{
    m_obj = obj;
    m_hasWireframe = false;

    log_trace("Detecting ObjectViewFlags for \"%s\"", m_obj->GetID().c_str());
    switch (m_obj->GetViewFlags()) {
    case ObjectViewFlag_Solid:
        m_display->SetDisplay(ViewportDisplay_Solid);
        break;
    case ObjectViewFlag_Textured:
        m_display->SetDisplay(ViewportDisplay_Textured);
        break;
    case ObjectViewFlag_Wire:
        m_display->SetDisplay(ViewportDisplay_Wire);
        break;
    case ObjectViewFlag_SolidWithWireframe:
        m_display->SetDisplay(ViewportDisplay_Solid);
        m_hasWireframe = true;
        break;
    case ObjectViewFlag_TexturedWithWireframe:
        m_display->SetDisplay(ViewportDisplay_Textured);
        m_hasWireframe = true;
        break;
    }

    m_display->SetWireframe(m_hasWireframe);

    auto tf = m_obj->GetTransform();
    m_transform->SetLocation(tf.location.x, tf.location.y, tf.location.z);
    m_transform->SetRotation(tf.rotation.x, tf.rotation.y, tf.rotation.z);
    m_transform->SetScale(tf.scale.x, tf.scale.y, tf.scale.z);
}

void MeshObjectPropertiesPane::OnSelectDisplay(wxCommandEvent& event)
{
    auto selection = event.GetSelection();
    log_trace("Changing ObjectViewFlags for \"%s\"", m_obj->GetID().c_str());
    switch (selection) {
    case ViewportDisplay_Solid:
        if (m_hasWireframe) {
            m_obj->SetViewFlags(ObjectViewFlag_SolidWithWireframe);
        } else {
            m_obj->SetViewFlags(ObjectViewFlag_Solid);
        }
        break;
    case ViewportDisplay_Textured:
        if (m_hasWireframe) {
            m_obj->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
        } else {
            m_obj->SetViewFlags(ObjectViewFlag_Textured);
        }
        break;
    case ViewportDisplay_Wire:
        m_obj->SetViewFlags(ObjectViewFlag_Wire);
        break;
    }
}

void MeshObjectPropertiesPane::OnCheckWireframe(wxCommandEvent& event)
{
    auto flags = m_obj->GetViewFlags();

    m_hasWireframe = event.GetInt();

    log_trace("Changing ObjectViewFlags for \"%s\"", m_obj->GetID().c_str());
    if (m_hasWireframe) {
        switch (flags) {
        case ObjectViewFlag_Solid:
            m_obj->SetViewFlags(ObjectViewFlag_SolidWithWireframe);
            break;
        case ObjectViewFlag_Textured:
            m_obj->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
            break;
        }
    } else {
        switch (flags) {
        case ObjectViewFlag_SolidWithWireframe:
            m_obj->SetViewFlags(ObjectViewFlag_Solid);
            break;
        case ObjectViewFlag_TexturedWithWireframe:
            m_obj->SetViewFlags(ObjectViewFlag_Textured);
            break;
        }
    }
}

void MeshObjectPropertiesPane::OnTransformLocation(Vec3Event& event)
{
    m_obj->SetLocation(event.GetX(), event.GetY(), event.GetZ());
}

void MeshObjectPropertiesPane::OnTransformRotation(Vec3Event& event)
{
    using namespace glm;
    m_obj->SetRotation(radians(event.GetX()), radians(event.GetY()), radians(event.GetZ()));
}

void MeshObjectPropertiesPane::OnTransformScale(Vec3Event& event)
{
    m_obj->SetScale(event.GetX(), event.GetY(), event.GetZ());
}
