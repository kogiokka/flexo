#include "pane/PropertiesPane.hpp"

#include "Project.hpp"
#include "gfx/Renderer.hpp"
#include "object/Object.hpp"
#include "object/ObjectList.hpp"
#include "pane/GuidesPropertiesPane.hpp"
#include "pane/MeshObjectPropertiesPane.hpp"

wxDEFINE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

PropertiesPane::PropertiesPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
    , m_props(nullptr)
    , m_project(project)
{
    m_project.Bind(EVT_PROPERTIES_PANE_OBJECT_SELECTED, &PropertiesPane::OnObjectSelected, this);
}

void PropertiesPane::OnObjectSelected(ObjectSelectEvent& event)
{
    auto id = event.GetId();
    auto type = event.GetType();

    if (type == ObjectType_Guides) {
        m_props = std::make_shared<GuidesPropertiesPane>();
    } else {
        m_props = std::make_shared<MeshObjectPropertiesPane>(*this, m_project);

        for (auto const& obj : ObjectList::Get(m_project)) {
            if (obj->GetID() == id) {
                m_props->BindObject(obj);
                break;
            }
        }
    }
}
