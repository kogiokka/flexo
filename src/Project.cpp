#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "AddDialog.hpp"
#include "EditableMesh.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "World.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/ObjectList.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "object/Cube.hpp"
#include "object/Grid.hpp"
#include "object/Guides.hpp"
#include "object/Plane.hpp"
#include "object/Sphere.hpp"
#include "object/SurfaceVoxels.hpp"
#include "object/Torus.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "util/Logger.h"

WatermarkingProject::WatermarkingProject()
    : m_frame {}
    , m_panel {}
    , m_model(nullptr)
{
    m_imageFile = "res/images/mandala.png";

#define X(evt, name) Bind(evt, &WatermarkingProject::OnMenuAdd, this);
    MENU_ADD_LIST
#undef X

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

    auto light = std::make_shared<Sphere>();
    light->SetLocation(0.0f, 5.0f, 0.0f);
    light->SetScale(0.2f, 0.2f, 0.2f);
    light->ApplyTransform();

    auto cube = std::make_shared<Cube>();
    cube->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));
    cube->SetViewFlags(ObjectViewFlag_Solid);
    objlist.Add(ObjectType_Cube, cube);
    objlist.Add(ObjectType_Light, light);
    objlist.Add(ObjectType_Guides, std::make_shared<Guides>());
    objlist.Submit(renderer);
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    if (m_model) {
        m_model->SetViewFlags(ObjectViewFlag_Solid);
    }
    ObjectList::Get(*this).Submit(Renderer::Get(*this));

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
    if (!m_model) {
        wxMessageDialog dlg(&ProjectWindow::Get(*this), "No volumetric model!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    float progress;
    auto status = m_model->Parameterize(*world.theMap, progress);

    wxProgressDialog dialog("Texture Mapping", "Please wait...", 100, &ProjectWindow::Get(*this),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    // After the parametrization done, regenerate the drawables to update texture coordinates.
    m_model->GenerateDrawables(Graphics::Get(*this));

    m_model->SetViewFlags(ObjectViewFlag_Textured);
    ObjectList::Get(*this).Submit(Renderer::Get(*this));
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

    world.theDataset = std::make_shared<Dataset<3>>(m_model->GetPositions());

    auto& objlist = ObjectList::Get(*this);
    objlist.Add(ObjectType_Model, m_model);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::OnMenuAdd(wxCommandEvent& event)
{
    auto* projwin = &ProjectWindow::Get(*this);
    auto& objlist = ObjectList::Get(*this);

    auto const id = event.GetId();
    std::shared_ptr<Object> obj;
    ObjectType type;

    if (id == EVT_MENU_ADD_PLANE) {
        AddDialog dlg(projwin, "Add Plane", 1);
        double size = 2.0f;
        auto* sizeCtrl = dlg.AddInputFloat("Size", size);
        if (dlg.ShowModal() == wxID_OK) {
            if (!sizeCtrl->GetValue().ToDouble(&size)) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Plane (size: %.3f)", size);

            obj = std::make_shared<Plane>(size);
            type = ObjectType_Plane;
        } else {
            return;
        }

    } else if (id == EVT_MENU_ADD_GRID) {
        AddDialog dlg(projwin, "Add Grid", 3);
        long xdiv = 10;
        long ydiv = 10;
        double size = 2.0f;
        auto* xdivCtrl = dlg.AddInputInteger("X Divisions", xdiv);
        auto* ydivCtrl = dlg.AddInputInteger("Y Divisions", ydiv);
        auto* sizeCtrl = dlg.AddInputFloat("Size", size);
        if (dlg.ShowModal() == wxID_OK) {
            if (!xdivCtrl->GetValue().ToLong(&xdiv) || !ydivCtrl->GetValue().ToLong(&ydiv)
                || !sizeCtrl->GetValue().ToDouble(&size)) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Grid (X-div: %ld, Y-div: %ld, size: %.3f)", size);

            obj = std::make_shared<Grid>(xdiv, ydiv, size);
            type = ObjectType_Grid;
        } else {
            return;
        }
    } else if (id == EVT_MENU_ADD_CUBE) {
        AddDialog dlg(projwin, "Add Cube", 1);
        double size = 2.0f;
        auto* sizeCtrl = dlg.AddInputFloat("Size", size);
        if (dlg.ShowModal() == wxID_OK) {
            if (!sizeCtrl->GetValue().ToDouble(&size)) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Cube (size: %.3f)", size);

            obj = std::make_shared<Cube>(size);
            type = ObjectType_Cube;
        } else {
            return;
        }
    } else if (id == EVT_MENU_ADD_UV_SPHERE) {
        AddDialog dlg(projwin, "Add UV Sphere", 3);
        long segs = 32;
        long rings = 16;
        double radius = 1.0;
        auto* segCtrl = dlg.AddInputInteger("Segments", segs);
        auto* ringCtrl = dlg.AddInputInteger("Rings", rings);
        auto* radCtrl = dlg.AddInputFloat("Radius", radius);
        if (dlg.ShowModal() == wxID_OK) {
            if (!segCtrl->GetValue().ToLong(&segs) || !ringCtrl->GetValue().ToLong(&rings)
                || !radCtrl->GetValue().ToDouble(&radius) || segs < 3 || rings < 3) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add UV Sphere (segments: %lu, rings: %lu, radius: %.3f)", segs, rings, radius);

            obj = std::make_shared<Sphere>(segs, rings, radius);
            type = ObjectType_Sphere;
        } else {
            return;
        }
    } else if (id == EVT_MENU_ADD_TORUS) {
        AddDialog dlg(projwin, "Add UV Sphere", 4);
        long mjSeg = 48;
        long mnSeg = 12;
        double mjRad = 1.0;
        double mnRad = 0.25;
        auto* mjSegCtrl = dlg.AddInputInteger("Major Segments", mjSeg);
        auto* mnSegCtrl = dlg.AddInputInteger("Minor Segments", mnSeg);
        auto* mjRadCtrl = dlg.AddInputFloat("Major Radius", mjRad);
        auto* mnRadCtrl = dlg.AddInputFloat("Minor Radius", mnRad);

        if (dlg.ShowModal() == wxID_OK) {
            if (!mjSegCtrl->GetValue().ToLong(&mjSeg) || !mnSegCtrl->GetValue().ToLong(&mnSeg)
                || !mjRadCtrl->GetValue().ToDouble(&mjRad) || !mnRadCtrl->GetValue().ToDouble(&mnRad) || mjSeg < 3
                || mnSeg < 3) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Torus: (major segments: %lu, minor segments: %lu, major radius: %.3f, minor radius: %.3f)",
                     mjSeg, mnSeg, mjRad, mnRad);

            obj = std::make_shared<Torus>(mjSeg, mnSeg, mjRad, mnRad);
            type = ObjectType_Torus;
        } else {
            return;
        }
    } else {
        return;
    }

    objlist.Add(type, obj);
    objlist.Submit(Renderer::Get(*this));
}
