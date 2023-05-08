#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "gfx/Renderer.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/TransfromWidget.hpp"
#include "pane/ViewportDisplayWidget.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

PropertiesPane::PropertiesPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
    , m_project(project)
{
    m_project.Bind(EVT_PROPERTIES_PANE_OBJECT_SELECTED, &PropertiesPane::OnObjectSelected, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_LOCATION, &PropertiesPane::OnTransformLocation, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_ROTATION, &PropertiesPane::OnTransformRotation, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_SCALE, &PropertiesPane::OnTransformScale, this);
    m_project.Bind(EVT_TRANSFORM_WIDGET_APPLY, [this](wxCommandEvent&) {
        m_obj->ApplyTransform();
        m_obj->GenerateDrawables(SceneViewportPane::Get(m_project).GetGL());
    });

    m_project.Bind(EVT_VIEWPORT_DISPLAY_WIDGET_CHECK_WIREFRAME, &PropertiesPane::OnCheckWireframe, this);
    m_project.Bind(EVT_VIEWPORT_DISPLAY_WIDGET_SELECT_DISPLAY, &PropertiesPane::OnSelectDisplay, this);

    m_transform = new TransformWidget(this, m_project);
    m_display = new ViewportDisplayWidget(this, m_project);

    AddGroup(m_transform);
    AddGroup(m_display);

    Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent&) {
        if (m_obj) {
            Enable();
        } else {
            Disable();
        }
    });
}

void PropertiesPane::OnObjectSelected(ObjectSelectEvent& event)
{
    GetObjectStatus(event.GetId());
}

void PropertiesPane::OnSelectDisplay(wxCommandEvent& event)
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

void PropertiesPane::OnCheckWireframe(wxCommandEvent& event)
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

void PropertiesPane::OnTransformLocation(Vec3Event& event)
{
    m_obj->SetLocation(event.GetX(), event.GetY(), event.GetZ());
}

void PropertiesPane::OnTransformRotation(Vec3Event& event)
{
    using namespace glm;
    m_obj->SetRotation(radians(event.GetX()), radians(event.GetY()), radians(event.GetZ()));
}

void PropertiesPane::OnTransformScale(Vec3Event& event)
{
    m_obj->SetScale(event.GetX(), event.GetY(), event.GetZ());
}

void PropertiesPane::GetObjectStatus(std::string const& id)
{
    for (auto const& obj : ObjectList::Get(m_project)) {
        if (obj->GetID() == id) {
            m_obj = obj;
            break;
        }
    }

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
