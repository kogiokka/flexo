#include <wx/valnum.h>

#include "Dataset.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "Scene.hpp"
#include "SceneController.hpp"
#include "VolumetricModelData.hpp"
#include "dialog/AddDialog.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "object/Object.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/SceneViewportPane.hpp"

#define X(evt, name) wxDEFINE_EVENT(evt, wxCommandEvent);
ADD_OBJECT_LIST
#undef X

wxDEFINE_EVENT(EVT_DELETE_OBJECT, wxCommandEvent);

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
    m_project.Bind(EVT_IMPORT_MODEL, &SceneController::OnImportModel, this);

    m_project.Bind(EVT_ADD_OBJECT_PLANE, &SceneController::OnAddPlane, this);
    m_project.Bind(EVT_ADD_OBJECT_GRID, &SceneController::OnAddGrid, this);
    m_project.Bind(EVT_ADD_OBJECT_CUBE, &SceneController::OnAddCube, this);
    m_project.Bind(EVT_ADD_OBJECT_UV_SPHERE, &SceneController::OnAddUVSphere, this);
    m_project.Bind(EVT_ADD_OBJECT_TORUS, &SceneController::OnAddTorus, this);
    m_project.Bind(EVT_ADD_OBJECT_MAP, &SceneController::OnAddMap, this);

    m_project.Bind(EVT_DELETE_OBJECT, &SceneController::OnDeleteObject, this);
}

void SceneController::CreateScene()
{
    SceneViewportPane::Get(m_project);
    Scene::Get(m_project).AddCube();
}

void SceneController::OnImportModel(wxCommandEvent& event)
{
    VolumetricModelData data;
    data.Read(event.GetString().ToStdString().c_str());
    Scene::Get(m_project).AddModel(data);
}

void SceneController::OnAddPlane(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add Plane", 1);
    double size = 2.0;
    auto ctrlSize = dlg.AddInputFloat("Size", size);

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!ctrlSize->GetValue().ToDouble(&size)) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
    }

    Scene::Get(m_project).AddPlane(size);
    log_info("Added Plane (size: %.3f)", size);
}

void SceneController::OnAddGrid(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add Grid", 3);
    long xdiv = 10;
    long ydiv = 10;
    double size = 2.0f;
    auto* xdivCtrl = dlg.AddInputInteger("X Divisions", xdiv);
    auto* ydivCtrl = dlg.AddInputInteger("Y Divisions", ydiv);
    auto* sizeCtrl = dlg.AddInputFloat("Size", size);

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!xdivCtrl->GetValue().ToLong(&xdiv) || !ydivCtrl->GetValue().ToLong(&ydiv)
        || !sizeCtrl->GetValue().ToDouble(&size)) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    Scene::Get(m_project).AddGrid(xdiv, ydiv, size);
    log_info("Added Grid (X-div: %ld, Y-div: %ld, size: %.3f)", xdiv, ydiv, size);
}

void SceneController::OnAddCube(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add Cube", 1);
    double size = 2.0f;
    auto* sizeCtrl = dlg.AddInputFloat("Size", size);
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!sizeCtrl->GetValue().ToDouble(&size)) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    Scene::Get(m_project).AddCube(size);
    log_info("Added Cube (size: %.3f)", size);
}

void SceneController::OnAddUVSphere(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add UV Sphere", 3);
    long segs = 32;
    long rings = 16;
    double radius = 1.0;
    auto* segCtrl = dlg.AddInputInteger("Segments", segs);
    auto* ringCtrl = dlg.AddInputInteger("Rings", rings);
    auto* radCtrl = dlg.AddInputFloat("Radius", radius);

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!segCtrl->GetValue().ToLong(&segs) || !ringCtrl->GetValue().ToLong(&rings)
        || !radCtrl->GetValue().ToDouble(&radius) || segs < 3 || rings < 3) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    Scene::Get(m_project).AddUVSphere(segs, rings, radius);

    log_info("Added UV Sphere (segments: %ld, rings: %ld, radius: %.3f)", segs, rings, radius);
}

void SceneController::OnAddTorus(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add UV Sphere", 4);
    long mjSeg = 48;
    long mnSeg = 12;
    double mjRad = 1.0;
    double mnRad = 0.25;
    auto* mjSegCtrl = dlg.AddInputInteger("Major Segments", mjSeg);
    auto* mnSegCtrl = dlg.AddInputInteger("Minor Segments", mnSeg);
    auto* mjRadCtrl = dlg.AddInputFloat("Major Radius", mjRad);
    auto* mnRadCtrl = dlg.AddInputFloat("Minor Radius", mnRad);

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!mjSegCtrl->GetValue().ToLong(&mjSeg) || !mnSegCtrl->GetValue().ToLong(&mnSeg)
        || !mjRadCtrl->GetValue().ToDouble(&mjRad) || !mnRadCtrl->GetValue().ToDouble(&mnRad) || mjSeg < 3
        || mnSeg < 3) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    Scene::Get(m_project).AddTorus(mjSeg, mnSeg, mjRad, mnRad);

    log_info("Added Torus: (major segments: %ld, minor segments: %ld, major radius: %.3f, minor radius: %.3f)", mjSeg,
             mnSeg, mjRad, mnRad);
}

void SceneController::OnAddMap(wxCommandEvent&)
{
    AddDialog dlg(m_project.GetWindow(), "Add Map (3 to 2)", 7);
    long width = 32;
    long height = 32;
    bool isCyclicX = false;
    bool isCyclicY = false;

    MapFlags flags = MapFlags_CyclicNone;
    int initState = 0;

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

    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    if (!widthCtrl->GetValue().ToLong(&width) || !heightCtrl->GetValue().ToLong(&height) || width < 2 || height < 2) {
        wxMessageDialog dlg(m_project.GetWindow(), "Invalid input(s)!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    isCyclicX = chkCyclicX->GetValue();
    isCyclicY = chkCyclicY->GetValue();

    if (isCyclicX) {
        flags |= MapFlags_CyclicX;
    }
    if (isCyclicY) {
        flags |= MapFlags_CyclicY;
    }

    for (auto* btn = initStatePlane->GetFirstInGroup(); btn != nullptr; btn = btn->GetNextInGroup()) {
        if (btn->GetValue()) {
            break;
        }
        ++initState;
    }

    Scene::Get(m_project).AddMap(width, height, flags, static_cast<MapInitState>(initState));

    log_info("Added Map: (width: %ld, height: %ld)", width, height);
}

void SceneController::OnDeleteObject(wxCommandEvent& event)
{
    Scene::Get(m_project).Delete(event.GetString().ToStdString());
}
