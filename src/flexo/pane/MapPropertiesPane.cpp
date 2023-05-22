#include "pane/MapPropertiesPane.hpp"
#include "Project.hpp"
#include "pane/PropertiesPane.hpp"
#include "pane/SceneViewportPane.hpp"

MapPropertiesPane::MapPropertiesPane(wxWindow* parent, FlexoProject& project)
    : MeshObjectPropertiesPane(parent, project)
{
    m_texture = new TextureWidget(this, m_project);
    GetSizer()->AddSpacer(3);
    GetSizer()->Add(m_texture, wxSizerFlags(0).Expand().Border(wxLEFT | wxRIGHT, 15));
}

MapPropertiesPane::~MapPropertiesPane()
{
}

void MapPropertiesPane::BindObject(std::shared_ptr<Object> obj)
{
    MeshObjectPropertiesPane::BindObject(obj);
}
