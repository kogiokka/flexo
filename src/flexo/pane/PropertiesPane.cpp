#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "gfx/Renderer.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/TransfromWidget.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

#define DISPLAY_TYPE_LIST                                                                                              \
    X(DisplayType_Solid, "Solid")                                                                                      \
    X(DisplayType_Textured, "Textured")                                                                                \
    X(DisplayType_Wire, "Wire")

#define X(type, name) type,
enum DisplayType : unsigned int { DISPLAY_TYPE_LIST };
#undef X

#define X(type, name) name,
static std::string DisplayTypeNames[] = { DISPLAY_TYPE_LIST };
#undef X

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

    m_transform = new TransformWidget(this, m_project);
    AddGroup(m_transform);

    auto* viewport = AddGroup("Viewport Display", 2);
    m_chkWire = viewport->AddCheckBoxWithHeading("Show", "Wireframe", false);
    m_combo = viewport->AddComboBox("Display As");

    for (auto const& name : DisplayTypeNames) {
        m_combo->Append(name);
    }
    m_combo->SetSelection(DisplayType_Solid);

    m_combo->Bind(wxEVT_COMBOBOX, &PropertiesPane::OnSelectDisplayType, this);
    m_chkWire->Bind(wxEVT_CHECKBOX, &PropertiesPane::OnCheckWireframe, this);

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

void PropertiesPane::OnSelectDisplayType(wxCommandEvent& event)
{
    auto selection = event.GetSelection();
    log_trace("Changing ObjectViewFlags for \"%s\"", m_obj->GetID().c_str());
    switch (selection) {
    case DisplayType_Solid:
        if (m_hasWireframe) {
            m_obj->SetViewFlags(ObjectViewFlag_SolidWithWireframe);
        } else {
            m_obj->SetViewFlags(ObjectViewFlag_Solid);
        }
        break;
    case DisplayType_Textured:
        if (m_hasWireframe) {
            m_obj->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
        } else {
            m_obj->SetViewFlags(ObjectViewFlag_Textured);
        }
        break;
    case DisplayType_Wire:
        m_obj->SetViewFlags(ObjectViewFlag_Wire);
        break;
    }
}

void PropertiesPane::OnCheckWireframe(wxCommandEvent&)
{
    auto flags = m_obj->GetViewFlags();

    m_hasWireframe = m_chkWire->GetValue();

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
        m_combo->SetSelection(DisplayType_Solid);
        break;
    case ObjectViewFlag_Textured:
        m_combo->SetSelection(DisplayType_Textured);
        break;
    case ObjectViewFlag_Wire:
        m_combo->SetSelection(DisplayType_Wire);
        break;
    case ObjectViewFlag_SolidWithWireframe:
        m_combo->SetSelection(DisplayType_Solid);
        m_hasWireframe = true;
        break;
    case ObjectViewFlag_TexturedWithWireframe:
        m_combo->SetSelection(DisplayType_Textured);
        m_hasWireframe = true;
        break;
    }

    m_chkWire->SetValue(m_hasWireframe);

    auto tf = m_obj->GetTransform();
    m_transform->SetLocation(tf.location.x, tf.location.y, tf.location.z);
    m_transform->SetRotation(tf.rotation.x, tf.rotation.y, tf.rotation.z);
    m_transform->SetScale(tf.scale.x, tf.scale.y, tf.scale.z);
}
