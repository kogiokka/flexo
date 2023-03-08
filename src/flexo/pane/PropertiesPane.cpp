#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "gfx/Renderer.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_PANE_OBJECT_CHANGED, wxCommandEvent);

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
    auto* transform = AddGroup("Transform", 10);
    m_transform.location.x = transform->AddInputText("Location X", "0");
    m_transform.location.y = transform->AddInputText("         Y", "0");
    m_transform.location.z = transform->AddInputText("         Z", "0");
    m_transform.rotation.x = transform->AddInputText("Rotoation X", "0");
    m_transform.rotation.y = transform->AddInputText("          Y", "0");
    m_transform.rotation.z = transform->AddInputText("          Z", "0");
    m_transform.scale.x = transform->AddInputText("Scale X", "1.0");
    m_transform.scale.y = transform->AddInputText("      Y", "1.0");
    m_transform.scale.z = transform->AddInputText("      Z", "1.0");

    m_transform.location.x->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformLocation, this);
    m_transform.location.y->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformLocation, this);
    m_transform.location.z->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformLocation, this);
    m_transform.rotation.x->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformRotation, this);
    m_transform.rotation.y->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformRotation, this);
    m_transform.rotation.z->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformRotation, this);
    m_transform.scale.x->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformScale, this);
    m_transform.scale.y->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformScale, this);
    m_transform.scale.z->Bind(wxEVT_TEXT, &PropertiesPane::OnTransformScale, this);

    auto* btnApply = transform->AddButton("Apply");
    btnApply->SetToolTip("Apply Object Transform");
    btnApply->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        m_obj->ApplyTransform();
        m_obj->GenerateDrawables(Graphics::Get(m_project));
        ObjectList::Get(m_project).Submit(Renderer::Get(m_project));
        m_transform.location.x->Clear();
        m_transform.location.y->Clear();
        m_transform.location.z->Clear();
        m_transform.rotation.x->Clear();
        m_transform.rotation.y->Clear();
        m_transform.rotation.z->Clear();
        m_transform.scale.x->Clear();
        m_transform.scale.y->Clear();
        m_transform.scale.z->Clear();
        *m_transform.location.x << 0.0f;
        *m_transform.location.y << 0.0f;
        *m_transform.location.z << 0.0f;
        *m_transform.rotation.x << 0.0f;
        *m_transform.rotation.y << 0.0f;
        *m_transform.rotation.z << 0.0f;
        *m_transform.scale.x << 1.0;
        *m_transform.scale.y << 1.0;
        *m_transform.scale.z << 1.0;
    });

    auto* viewport = AddGroup("Viewport Display", 2);
    m_chkWire = viewport->AddCheckBoxWithHeading("Show", "Wireframe", false);
    m_combo = viewport->AddComboBox("Display As");

    for (auto const& name : DisplayTypeNames) {
        m_combo->Append(name);
    }
    m_combo->SetSelection(DisplayType_Solid);

    m_combo->Bind(wxEVT_COMBOBOX, &PropertiesPane::OnSelectDisplayType, this);
    m_chkWire->Bind(wxEVT_CHECKBOX, &PropertiesPane::OnCheckWireframe, this);
    m_project.Bind(EVT_PROPERTIES_PANE_OBJECT_CHANGED, &PropertiesPane::OnObjectChanged, this);

    Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent&) {
        if (m_obj) {
            Enable();
        } else {
            Disable();
        }
    });
}

void PropertiesPane::OnObjectChanged(wxCommandEvent& event)
{
    GetObjectStatus(event.GetString().ToStdString());
}

void PropertiesPane::OnSelectDisplayType(wxCommandEvent& event)
{
    auto selection = event.GetSelection();
    auto& objlist = ObjectList::Get(m_project);
    auto& renderer = Renderer::Get(m_project);
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
    objlist.Submit(renderer);
}

void PropertiesPane::OnCheckWireframe(wxCommandEvent&)
{
    auto& objlist = ObjectList::Get(m_project);
    auto& renderer = Renderer::Get(m_project);
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
    objlist.Submit(renderer);
}

void PropertiesPane::OnTransformLocation(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_transform.location;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        m_obj->SetLocation(x, y, z);
    }
}

void PropertiesPane::OnTransformRotation(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_transform.rotation;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        m_obj->SetRotation(glm::radians(x), glm::radians(y), glm::radians(z));
    }
}

void PropertiesPane::OnTransformScale(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_transform.scale;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        m_obj->SetScale(x, y, z);
    }
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

    // Transform
    auto tf = m_obj->GetTransform();
    m_transform.location.x->Clear();
    m_transform.location.y->Clear();
    m_transform.location.z->Clear();
    m_transform.rotation.x->Clear();
    m_transform.rotation.y->Clear();
    m_transform.rotation.z->Clear();
    m_transform.scale.x->Clear();
    m_transform.scale.y->Clear();
    m_transform.scale.z->Clear();
    *m_transform.location.x << tf.location.x;
    *m_transform.location.y << tf.location.y;
    *m_transform.location.z << tf.location.z;
    *m_transform.rotation.x << tf.rotation.x;
    *m_transform.rotation.y << tf.rotation.y;
    *m_transform.rotation.z << tf.rotation.z;
    *m_transform.scale.x << tf.scale.x;
    *m_transform.scale.y << tf.scale.y;
    *m_transform.scale.z << tf.scale.z;
}
