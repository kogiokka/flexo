#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SceneController.hpp"
#include "VolumetricModelData.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/SceneViewportPane.hpp"

FlexoProject::FlexoProject()
    : m_frame {}
{
}

FlexoProject::~FlexoProject()
{
}

void FlexoProject::SetWindow(wxFrame* frame)
{
    m_frame = frame;
}

wxFrame* FlexoProject::GetWindow()
{
    return m_frame;
}

void FlexoProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    auto model = std::make_shared<SurfaceVoxels>(data);
    model->SetViewFlags(ObjectViewFlag_Solid);
    model->SetTexture(Bind::TextureManager::Resolve(SceneViewportPane::Get(*this).GetGL(), "images/blank.png", 0));

    log_info("%lu voxels will be rendered.", model->Voxels().size());

    SceneController::Get(*this).AcceptObject(model);
}
