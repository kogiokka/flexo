#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "SceneController.hpp"
#include "gfx/Renderer.hpp"
#include "object/Object.hpp"
#include "pane/MapPropertiesPane.hpp"
#include "pane/MeshObjectPropertiesPane.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

PropertiesPane::PropertiesPane(wxWindow* parent, FlexoProject& project)
    : wxScrolledWindow(parent)
    , m_props(nullptr)
    , m_layout(nullptr)
    , m_project(project)
{
    m_project.Bind(EVT_PROPERTIES_PANE_OBJECT_SELECTED, &PropertiesPane::OnObjectSelected, this);

    m_layout = new wxBoxSizer(wxVERTICAL);
    SetSizer(m_layout);
    SetScrollRate(10, 10);
}

void PropertiesPane::OnObjectSelected(ObjectSelectEvent& event)
{
    auto id = event.GetId();
    auto type = event.GetType();

    if (m_props) {
        m_props->Destroy();
    }

    if (type == ObjectType_Map) {
        m_props = new MapPropertiesPane(this, m_project);
    } else {
        m_props = new MeshObjectPropertiesPane(this, m_project);
    }

    if (auto obj = SceneController::Get(m_project).FindObject(id).lock()) {
        m_props->BindObject(obj);
    }

    m_layout->Add(m_props, wxSizerFlags(0).Expand());
}
