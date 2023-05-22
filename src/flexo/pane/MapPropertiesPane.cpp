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

    m_project.Bind(EVT_TEXTURE_WIDGET_OPEN_IMAGE, &MapPropertiesPane::OnOpenImage, this);
}

MapPropertiesPane::~MapPropertiesPane()
{
}

void MapPropertiesPane::BindObject(std::shared_ptr<Object> obj)
{
    MeshObjectPropertiesPane::BindObject(obj);
    m_texture->SetTextureName(obj->GetTexture()->GetName());
}

void MapPropertiesPane::OnOpenImage(wxCommandEvent& event)
{
    auto& gfx = SceneViewportPane::Get(m_project).GetGL();
    std::string const filename = event.GetString().ToStdString();

    m_obj->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
    m_obj->GenerateDrawables(gfx);
}
