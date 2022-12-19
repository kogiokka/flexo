#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "EditableMesh.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "gfx/DrawList.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/LightSource.hpp"
#include "gfx/drawable/MapVertex.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/VolumetricModel.hpp"
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

    // FIXME
    Bind(EVT_OPEN_IMAGE, [this](wxCommandEvent& event) {
        m_imageFile = event.GetString().ToStdString();

        auto& gfx = Graphics::Get(*this);
        std::string const filename = event.GetString().ToStdString();
        auto& drawables = DrawList::Get(*this);
        for (auto it = drawables.begin(); it != drawables.end(); it++) {
            auto tex = Bind::TextureManager::Resolve(gfx, filename.c_str(), 0);
            TexturedDrawable* face = dynamic_cast<TexturedDrawable*>(it->get());
            if (face != nullptr) {
                face->ChangeTexture(tex);
            }
            VolumetricModel* model = dynamic_cast<VolumetricModel*>(it->get());
            if (model != nullptr) {
                model->ChangeTexture(tex);
            }
        }

        drawables.Submit(Renderer::Get(*this));
    });
}

WatermarkingProject::~WatermarkingProject()
{
}

void WatermarkingProject::CreateScene()
{
    auto& drawlist = DrawList::Get(*this);
    auto& gfx = Graphics::Get(*this);
    drawlist.Add(ObjectType_Light, std::make_shared<LightSource>(gfx, ConstructSphere(32, 16).GenerateMesh()));
    drawlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

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

    auto model = std::make_shared<VolumetricModel>(Graphics::Get(*this), m_model->GenMesh());
    model->ChangeTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));
    SetModelDrawable(model);
}

void WatermarkingProject::OnSOMPaneMapChanged(wxCommandEvent&)
{
    auto emesh = GenMapEditableMesh(*world.theMap);
    world.mapMesh = emesh;

    auto& gfx = Graphics::Get(*this);
    auto& drawlist = DrawList::Get(*this);

    drawlist.Add(ObjectType_MapVertex,
                 std::make_shared<MapVertex>(gfx, ConstructSphere(32, 16).GenerateMesh(), emesh.positions));
    drawlist.Add(ObjectType_MapEdge, std::make_shared<WireDrawable>(gfx, emesh.GenerateWireframe()));
    drawlist.Add(ObjectType_MapFace,
                 std::make_shared<TexturedDrawable>(gfx, emesh.GenerateMesh(),
                                                    Bind::TextureManager::Resolve(gfx, m_imageFile.c_str(), 0)));
    drawlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    m_model = std::make_unique<SurfaceVoxels>(data);
    log_info("%lu voxels will be rendered.", m_model->Voxels().size());

    world.theDataset = std::make_shared<Dataset<3>>(m_model->GenPositions());
    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), m_model->GenMesh()));
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

        EditableMesh sphere = ConstructSphere(60, 60);

        EditableMesh::TransformStack stack;
        stack.PushTranslate(center);
        stack.PushScale(radius, radius, radius);
        stack.Apply(sphere);

        world.theDataset = std::make_shared<Dataset<3>>(sphere.positions);

        SetModelDrawable(std::make_shared<SolidDrawable>(Graphics::Get(*this), sphere.GenerateMesh()));
    }
}

void WatermarkingProject::SetModelDrawable(std::shared_ptr<DrawableBase> drawable)
{
    auto& drawlist = DrawList::Get(*this);
    drawlist.Add(ObjectType_Model, drawable);
    drawlist.Submit(Renderer::Get(*this));
}
