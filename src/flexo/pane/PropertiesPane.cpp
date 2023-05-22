#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "gfx/Renderer.hpp"
#include "object/Object.hpp"
#include "object/ObjectList.hpp"
#include "pane/GuidesPropertiesPane.hpp"
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

    if (type == ObjectType_Guides) {
        m_props = new GuidesPropertiesPane(this, m_project);
    } else if (type == ObjectType_Map) {
        m_props = new MapPropertiesPane(this, m_project);
    } else {
        m_props = new MeshObjectPropertiesPane(this, m_project);
    }

    for (auto const& obj : ObjectList::Get(m_project)) {
        if (obj->GetID() == id) {
            m_props->BindObject(obj);
            break;
        }
    }

    m_layout->Add(m_props, wxSizerFlags(0).Expand());
}
