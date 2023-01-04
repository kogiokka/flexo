#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/valnum.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "AddDialog.hpp"
#include "EditableMesh.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "RandomRealNumber.hpp"
#include "SelfOrganizingMap.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "object/Cube.hpp"
#include "object/Grid.hpp"
#include "object/Guides.hpp"
#include "object/ObjectList.hpp"
#include "object/Plane.hpp"
#include "object/Sphere.hpp"
#include "object/SurfaceVoxels.hpp"
#include "object/Torus.hpp"
#include "util/Logger.h"

FlexoProject::FlexoProject()
    : m_frame {}
    , m_panel {}
    , theDataset(nullptr)
    , theMap()
    , theModel()
{
    m_imageFile = "res/images/mandala.png";

#define X(evt, name) Bind(evt, &FlexoProject::OnMenuAdd, this);
    MENU_ADD_LIST
#undef X

    Bind(EVT_OPEN_IMAGE, [this](wxCommandEvent& event) {
        m_imageFile = event.GetString().ToStdString();

        auto& gfx = Graphics::Get(*this);
        auto& objlist = ObjectList::Get(*this);
        std::string const filename = event.GetString().ToStdString();

        if (auto map = theMap.lock()) {
            map->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            map->GenerateDrawables(gfx);
        }
        if (auto model = theModel.lock()) {
            model->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            model->GenerateDrawables(gfx);
        }

        objlist.Submit(Renderer::Get(*this));
    });
}

FlexoProject::~FlexoProject()
{
}

void FlexoProject::CreateScene()
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

void FlexoProject::CreateProject()
{
    if (auto it = theModel.lock()) {
        it->SetViewFlags(ObjectViewFlag_Solid);
    }
    ObjectList::Get(*this).Submit(Renderer::Get(*this));

    SelfOrganizingMap::Get(*this).CreateProcedure(theMap.lock(), theDataset);
}

void FlexoProject::StopProject()
{
    auto& som = SelfOrganizingMap::Get(*this);
    if (som.IsTraining()) {
        som.ToggleTraining();
    }
    som.StopWorker();
}

void FlexoProject::SetFrame(wxFrame* frame)
{
    m_frame = frame;
}

void FlexoProject::SetPanel(wxWindow* panel)
{
    m_panel = panel;
}

wxWindow* FlexoProject::GetPanel()
{
    return m_panel;
}

void FlexoProject::DoParameterization()
{
    auto model = std::dynamic_pointer_cast<SurfaceVoxels>(theModel.lock());
    if (!model) {
        wxMessageDialog dlg(&ProjectWindow::Get(*this), "Not a volumetric model!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    float progress;
    auto status = model->Parameterize(*(theMap.lock()), progress);

    wxProgressDialog dialog("Parameterizing", "Please wait...", 100, &ProjectWindow::Get(*this),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    // After the parametrization done, regenerate the drawables to update texture coordinates.
    model->SetViewFlags(ObjectViewFlag_Textured);
    model->GenerateMesh();
    model->GenerateDrawables(Graphics::Get(*this));
    ObjectList::Get(*this).Submit(Renderer::Get(*this));
}

void FlexoProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    auto model = std::make_shared<SurfaceVoxels>(data);
    model->SetViewFlags(ObjectViewFlag_Solid);
    model->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));

    log_info("%lu voxels will be rendered.", model->Voxels().size());

    theModel = model;

    auto& objlist = ObjectList::Get(*this);
    objlist.Add(ObjectType_Model, theModel.lock());
    objlist.Submit(Renderer::Get(*this));
}

void FlexoProject::OnMenuAdd(wxCommandEvent& event)
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
            log_info("Add UV Sphere (segments: %ld, rings: %ld, radius: %.3f)", segs, rings, radius);

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
            log_info("Add Torus: (major segments: %ld, minor segments: %ld, major radius: %.3f, minor radius: %.3f)",
                     mjSeg, mnSeg, mjRad, mnRad);

            obj = std::make_shared<Torus>(mjSeg, mnSeg, mjRad, mnRad);
            type = ObjectType_Torus;
        } else {
            return;
        }
    } else if (id == EVT_MENU_ADD_MAP) {
        AddDialog dlg(projwin, "Add Map (3 to 2)", 6);
        long width = 32;
        long height = 32;
        bool isCyclicX = false;
        bool isCyclicY = false;
        MapFlags flags = MapFlags_CyclicNone;
        MapInitState state = MapInitState_Random;

        auto* widthCtrl = dlg.AddInputInteger("Width", width);
        auto* heightCtrl = dlg.AddInputInteger("Height", height);
        auto* chkCyclicX = dlg.AddCheckBoxWithHeading("Cyclic on", "X", isCyclicX);
        auto* chkCyclicY = dlg.AddCheckBox("Y", isCyclicY);
        auto* initStatePlane = dlg.AddRadioButtonWithHeading("Initialize Method", "Plane", true);
        dlg.AddRadioButton("Random", false);

        wxIntegerValidator<int> validDimen;
        validDimen.SetRange(1, 512);
        widthCtrl->SetValidator(validDimen);
        heightCtrl->SetValidator(validDimen);

        if (dlg.ShowModal() == wxID_OK) {
            isCyclicX = chkCyclicX->GetValue();
            isCyclicY = chkCyclicY->GetValue();
            if (isCyclicX) {
                flags |= MapFlags_CyclicX;
            }
            if (isCyclicY) {
                flags |= MapFlags_CyclicY;
            }
            if (widthCtrl->GetValue().ToLong(&width) && heightCtrl->GetValue().ToLong(&height)) {
                if (initStatePlane->GetValue()) {
                    state = MapInitState_Plane;
                }

                BoundingBox box = { { 5.0f, 5.0f, 5.0f }, { -5.0f, -5.0f, -5.0f } };
                if (auto it = theDataset) {
                    box = it->GetBoundingBox();
                }

                auto map = std::make_shared<Map<3, 2>>();
                float const w = static_cast<float>(width - 1);
                float const h = static_cast<float>(height - 1);

                if (state == MapInitState_Random) {
                    RandomRealNumber<float> xRng(box.min.x, box.max.x);
                    RandomRealNumber<float> yRng(box.min.y, box.max.y);
                    RandomRealNumber<float> zRng(box.min.z, box.max.z);

                    for (int j = 0; j < height; ++j) {
                        for (int i = 0; i < width; ++i) {
                            map->nodes.emplace_back(Vec3f { xRng.scalar(), yRng.scalar(), zRng.scalar() },
                                                    Vec2f { static_cast<float>(i), static_cast<float>(j) },
                                                    Vec2f { i / w, j / h });
                        }
                    }
                } else if (state == MapInitState_Plane) {
                    float dx = (box.max.x - box.min.x) / static_cast<float>(width);
                    float dy = (box.max.y - box.min.y) / static_cast<float>(height);

                    for (int j = 0; j < height; ++j) {
                        for (int i = 0; i < width; ++i) {
                            map->nodes.emplace_back(Vec3f { box.min.x + i * dx, box.min.y + j * dy, box.max.z },
                                                    Vec2f { static_cast<float>(i), static_cast<float>(j) },
                                                    Vec2f { i / w, j / h });
                        }
                    }
                }

                map->size.x = width;
                map->size.y = height;
                map->flags = flags;

                map->GenerateMesh();
                obj = map;
            }

            if (!widthCtrl->GetValue().ToLong(&width) || !heightCtrl->GetValue().ToLong(&height) || width < 3
                || height < 3) {
                wxMessageDialog dlg(projwin, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Map: (width: %ld, height: %ld)", width, height);

            obj->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));
            obj->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
            type = ObjectType_Map;
        } else {
            return;
        }

    } else {
        return;
    }

    objlist.Add(type, obj);
    objlist.Submit(Renderer::Get(*this));
}
