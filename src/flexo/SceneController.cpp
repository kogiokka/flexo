#include <wx/valnum.h>

#include "Dataset.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "RandomRealNumber.hpp"
#include "SceneController.hpp"
#include "VolumetricModelData.hpp"
#include "dialog/AddDialog.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "object/Object.hpp"
#include "object/ObjectList.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/SceneViewportPane.hpp"

constexpr auto PI = 3.14159265358979323846;

#define X(evt, name) wxDEFINE_EVENT(evt, wxCommandEvent);
ADD_OBJECT_LIST
#undef X

// Register factory: SceneController
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> SharedPtr<SceneController> { return std::make_shared<SceneController>(project); }
};

SceneController& SceneController::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<SceneController>(factoryKey);
}

SceneController const& SceneController::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

SceneController::SceneController(FlexoProject& project)
    : m_project(project)
{
#define X(evt, name) m_project.Bind(evt, &SceneController::OnAddObject, this);
    ADD_OBJECT_LIST
#undef X

    m_project.Bind(EVT_IMPORT_MODEL, &SceneController::OnImportModel, this);
}

void SceneController::CreateScene()
{
    auto& viewport = SceneViewportPane::Get(m_project);

    auto cube = std::make_shared<Object>(ObjectType_Cube, ConstructCube());
    cube->SetTexture(Bind::TextureManager::Resolve(viewport.GetGL(), "images/blank.png", 0));
    cube->SetViewFlags(ObjectViewFlag_Solid);

    AcceptObject(cube);
}

void SceneController::AcceptObject(std::shared_ptr<Object> object)
{
    ObjectList::Get(m_project).Add(object);
}

void SceneController::OnAddObject(wxCommandEvent& event)
{
    auto* parent = m_project.GetWindow();

    auto const id = event.GetId();
    std::shared_ptr<Object> obj;

    if (id == EVT_ADD_OBJECT_PLANE) {
        PlaneAddDialog dlg(parent);
        if (dlg.ShowModal() == wxID_OK) {
            double size = dlg.GetSize();
            obj = std::make_shared<Object>(ObjectType_Plane, ConstructPlane(size));
        } else {
            return;
        }

    } else if (id == EVT_ADD_OBJECT_GRID) {
        AddDialog dlg(parent, "Add Grid", 3);
        long xdiv = 10;
        long ydiv = 10;
        double size = 2.0f;
        auto* xdivCtrl = dlg.AddInputInteger("X Divisions", xdiv);
        auto* ydivCtrl = dlg.AddInputInteger("Y Divisions", ydiv);
        auto* sizeCtrl = dlg.AddInputFloat("Size", size);
        if (dlg.ShowModal() == wxID_OK) {
            if (!xdivCtrl->GetValue().ToLong(&xdiv) || !ydivCtrl->GetValue().ToLong(&ydiv)
                || !sizeCtrl->GetValue().ToDouble(&size)) {
                wxMessageDialog dlg(parent, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Grid (X-div: %ld, Y-div: %ld, size: %.3f)", xdiv, ydiv, size);

            obj = std::make_shared<Object>(ObjectType_Grid, ConstructGrid(xdiv, ydiv, size));
        } else {
            return;
        }
    } else if (id == EVT_ADD_OBJECT_CUBE) {
        AddDialog dlg(parent, "Add Cube", 1);
        double size = 2.0f;
        auto* sizeCtrl = dlg.AddInputFloat("Size", size);
        if (dlg.ShowModal() == wxID_OK) {
            if (!sizeCtrl->GetValue().ToDouble(&size)) {
                wxMessageDialog dlg(parent, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Cube (size: %.3f)", size);

            obj = std::make_shared<Object>(ObjectType_Cube, ConstructCube(size));
        } else {
            return;
        }
    } else if (id == EVT_ADD_OBJECT_UV_SPHERE) {
        AddDialog dlg(parent, "Add UV Sphere", 3);
        long segs = 32;
        long rings = 16;
        double radius = 1.0;
        auto* segCtrl = dlg.AddInputInteger("Segments", segs);
        auto* ringCtrl = dlg.AddInputInteger("Rings", rings);
        auto* radCtrl = dlg.AddInputFloat("Radius", radius);
        if (dlg.ShowModal() == wxID_OK) {
            if (!segCtrl->GetValue().ToLong(&segs) || !ringCtrl->GetValue().ToLong(&rings)
                || !radCtrl->GetValue().ToDouble(&radius) || segs < 3 || rings < 3) {
                wxMessageDialog dlg(parent, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add UV Sphere (segments: %ld, rings: %ld, radius: %.3f)", segs, rings, radius);

            obj = std::make_shared<Object>(ObjectType_Sphere, ConstructSphere(segs, rings, radius));
        } else {
            return;
        }
    } else if (id == EVT_ADD_OBJECT_TORUS) {
        AddDialog dlg(parent, "Add UV Sphere", 4);
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
                wxMessageDialog dlg(parent, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Torus: (major segments: %ld, minor segments: %ld, major radius: %.3f, minor radius: %.3f)",
                     mjSeg, mnSeg, mjRad, mnRad);

            obj = std::make_shared<Object>(ObjectType_Torus, ConstructTorus(mjSeg, mnSeg, mjRad, mnRad));
        } else {
            return;
        }
    } else if (id == EVT_ADD_OBJECT_MAP) {
        AddDialog dlg(parent, "Add Map (3 to 2)", 7);
        long width = 32;
        long height = 32;
        bool isCyclicX = false;
        bool isCyclicY = false;
        MapFlags flags = MapFlags_CyclicNone;

        enum MapInitState {
            MapInitState_Plane = 0,
            MapInitState_Cylinder,
            MapInitState_Random,
        };

        int state = 0;

        auto* widthCtrl = dlg.AddInputInteger("Width", width);
        auto* heightCtrl = dlg.AddInputInteger("Height", height);
        auto* chkCyclicX = dlg.AddCheckBoxWithHeading("Cyclic on", "X", isCyclicX);
        auto* chkCyclicY = dlg.AddCheckBox("Y", isCyclicY);
        auto* initStatePlane = dlg.AddRadioButtonWithHeading("Initialize Method", "Plane", true);
        dlg.AddRadioButton("Cylinder", false);
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

            auto& gfx = SceneViewportPane::Get(m_project).GetGL();

            if (widthCtrl->GetValue().ToLong(&width) && heightCtrl->GetValue().ToLong(&height)) {

                for (auto* btn = initStatePlane->GetFirstInGroup(); btn != nullptr; btn = btn->GetNextInGroup()) {
                    if (btn->GetValue()) {
                        break;
                    }
                    ++state;
                }

                BoundingBox box = { { 5.0f, 5.0f, 5.0f }, { -5.0f, -5.0f, -5.0f } };

                auto map = std::make_shared<Map<3, 2>>();
                float const w = static_cast<float>(width - 1);
                float const h = static_cast<float>(height - 1);

                switch (static_cast<MapInitState>(state)) {
                default:
                case MapInitState_Random: {
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
                } break;
                case MapInitState_Plane: {
                    float dx = 2.0f / static_cast<float>(width - 1);
                    float dy = 2.0f / static_cast<float>(height - 1);

                    for (int j = 0; j < height; ++j) {
                        for (int i = 0; i < width; ++i) {
                            map->nodes.emplace_back(Vec3f { -1.0f + i * dx, -1.0f + j * dy, 0.005f },
                                                    Vec2f { static_cast<float>(i), static_cast<float>(j) },
                                                    Vec2f { i / w, j / h });
                        }
                    }
                } break;
                case MapInitState_Cylinder: {
                    float dr = glm::radians(360.0f) / static_cast<float>(width - 1);
                    float dz = 2.0f * PI * 1.0f / static_cast<float>(height - 1);

                    for (int j = 0; j < height; ++j) {
                        for (int i = 0; i < width; ++i) {
                            auto rad = i * dr;
                            map->nodes.emplace_back(Vec3f { cos(rad), sin(rad), j * dz },
                                                    Vec2f { static_cast<float>(i), static_cast<float>(j) },
                                                    Vec2f { i / w, j / h });
                        }
                    }
                } break;
                }

                map->size.x = width;
                map->size.y = height;
                map->flags = flags;

                map->GenerateDrawables(gfx);
                obj = map;
            }

            if (!widthCtrl->GetValue().ToLong(&width) || !heightCtrl->GetValue().ToLong(&height) || width < 2
                || height < 2) {
                wxMessageDialog dlg(parent, "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
                dlg.ShowModal();
                return;
            }
            log_info("Add Map: (width: %ld, height: %ld)", width, height);

            obj->SetTexture(Bind::TextureManager::Resolve(gfx, "images/blank.png", 0));
            obj->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
        } else {
            return;
        }

    } else {
        return;
    }

    AcceptObject(obj);
}

void SceneController::OnImportModel(wxCommandEvent& event)
{
    VolumetricModelData data;
    data.Read(event.GetString().ToStdString().c_str());

    auto model = std::make_shared<SurfaceVoxels>(data);
    model->SetViewFlags(ObjectViewFlag_Solid);
    model->SetTexture(Bind::TextureManager::Resolve(SceneViewportPane::Get(m_project).GetGL(), "images/blank.png", 0));

    log_info("%lu voxels will be rendered.", model->Voxels().size());

    AcceptObject(model);
}
