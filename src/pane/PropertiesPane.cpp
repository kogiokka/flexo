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

PropertiesPane::PropertiesPane(wxWindow* parent, WatermarkingProject& project)
    : ControlsPaneBase(parent, project)
    , m_project(project)
{
    auto* group = AddGroup("Viewport Display", 2);

    m_chkWire = group->AddCheckBoxWithHeading("Show", "Wireframe", false);
    m_combo = group->AddComboBox("Display As");

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
}
