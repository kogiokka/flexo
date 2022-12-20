#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "EditableMesh.hpp"
#include "Guides.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "Sphere.hpp"
#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/ObjectList.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "util/Logger.h"

WatermarkingProject::WatermarkingProject()
    : m_frame {}
    , m_panel {}
    , m_model(nullptr)
{
    m_imageFile = "res/images/mandala.png";
    Bind(EVT_ADD_UV_SPHERE, &WatermarkingProject::OnMenuAddModel, this);
    Bind(EVT_SOM_PANE_MAP_CHANGED, &WatermarkingProject::OnSOMPaneMapChanged, this);

    Bind(EVT_OPEN_IMAGE, [this](wxCommandEvent& event) {
        m_imageFile = event.GetString().ToStdString();

        auto& gfx = Graphics::Get(*this);
        auto& objlist = ObjectList::Get(*this);
        std::string const filename = event.GetString().ToStdString();

        if (m_model) {
            m_model->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            objlist.Add(ObjectType_Model, m_model);
            objlist.Submit(Renderer::Get(*this));
        }

        if (world.theMap) {
            world.theMap->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            objlist.Add(ObjectType_Map, world.theMap);
            objlist.Submit(Renderer::Get(*this));
        }
    });
}

WatermarkingProject::~WatermarkingProject()
{
}

void WatermarkingProject::CreateScene()
{
    auto& objlist = ObjectList::Get(*this);
    auto& renderer = Renderer::Get(*this);

    EditableMesh::TransformStack tf;
    tf.PushTranslate(0.0f, 5.0f, 0.0f);
    tf.PushScale(0.2f, 0.2f, 0.2f);
    auto light = std::make_shared<Sphere>();
    light->SetTransform(tf);

    objlist.Add(ObjectType_Light, light);
    objlist.Add(ObjectType_Guides, std::make_shared<Guides>());
    objlist.Submit(renderer);
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    auto& objlist = ObjectList::Get(*this);
    m_model->SetViewFlags(ObjectViewFlag_Solid);
    objlist.Add(ObjectType_Model, m_model);
    objlist.Submit(Renderer::Get(*this));

    SelfOrganizingMap::Get(*this).CreateProcedure(world.theMap, world.theDataset);
}

void WatermarkingProject::StopProject()
{
    auto& som = SelfOrganizingMap::Get(*this);
    if (som.IsTraining()) {
        som.ToggleTraining();
    }
    som.StopWorker();
}

void WatermarkingProject::SetFrame(wxFrame* frame)
{
    m_frame = frame;
}

void WatermarkingProject::SetPanel(wxWindow* panel)
{
    m_panel = panel;
}

wxWindow* WatermarkingProject::GetPanel()
{
    return m_panel;
}

void WatermarkingProject::DoWatermark()
{
    float progress;
    auto status = m_model->Parameterize(*world.theMap, progress);

    wxProgressDialog dialog("Texture Mapping", "Please wait...", 100, &ProjectWindow::Get(*this),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    m_model->SetViewFlags(ObjectViewFlag_Textured);

    auto& objlist = ObjectList::Get(*this);
    objlist.Add(ObjectType_Model, m_model);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::OnSOMPaneMapChanged(wxCommandEvent&)
{
    auto& gfx = Graphics::Get(*this);
    auto& objlist = ObjectList::Get(*this);

    world.theMap->SetTexture(Bind::TextureManager::Resolve(gfx, m_imageFile.c_str(), 0));
    world.theMap->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
    objlist.Add(ObjectType_Map, world.theMap);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    m_model = std::make_shared<SurfaceVoxels>(data);
    m_model->SetViewFlags(ObjectViewFlag_Solid);
    m_model->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));

    log_info("%lu voxels will be rendered.", m_model->Voxels().size());

    world.theDataset = std::make_shared<Dataset<3>>(m_model->GenPositions());

    auto& objlist = ObjectList::Get(*this);
    objlist.Add(ObjectType_Model, m_model);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::OnMenuAddModel(wxCommandEvent& event)
{
    if (world.theDataset == nullptr) {
        wxMessageDialog dialog(&ProjectWindow::Get(*this), "Please import a model from the File menu first.", "Error",
                               wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    // FIXME Better way to get the bounding box
    BoundingBox const bbox = world.theDataset->GetBoundingBox();

    if (event.GetId() == EVT_ADD_UV_SPHERE) {
        auto const& [max, min] = bbox;
        float const radius = glm::length((max - min) * 0.5f);
        glm::vec3 const center = (max + min) * 0.5f;

        auto sphere = std::make_shared<Sphere>(60, 60);

        EditableMesh::TransformStack stack;
        stack.PushTranslate(center);
        stack.PushScale(radius, radius, radius);
        sphere->SetTransform(stack);

        auto& objlist = ObjectList::Get(*this);
        objlist.Add(ObjectType_Sphere, sphere);
        objlist.Submit(Renderer::Get(*this));
    }
}
