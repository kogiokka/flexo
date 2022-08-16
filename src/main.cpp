#include <cmath>
#include <memory>

#include <wx/msgdlg.h>
#include <wx/textctrl.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Mesh.hpp"
#include "ProjectWindow.hpp"
#include "Renderer.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"
#include "pane/LatticePane.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "pane/WatermarkingPane.hpp"

wxBEGIN_EVENT_TABLE(WatermarkingApp, wxApp)
    EVT_COMMAND(wxID_ANY, EVT_TOGGLE_RENDER_FLAG, WatermarkingApp::OnToggleRenderOption)
    EVT_COMMAND(wxID_ANY, EVT_TOGGLE_LATTICE_FLAG, WatermarkingApp::OnToggleLatticeFlag)
    EVT_COMMAND(wxID_ANY, EVT_GENERATE_MODEL_DOME, WatermarkingApp::OnMenuGenerateModel)
    EVT_COMMAND(wxID_ANY, EVT_IMPORT_MODEL, WatermarkingApp::OnMenuImportModel)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(WatermarkingApp);

WatermarkingApp::WatermarkingApp()
{
}

void WatermarkingApp::OnUnhandledException()
{
    throw;
}

int WatermarkingApp::OnExit()
{
    SelfOrganizingMap::Get(*m_project).StopWorker();
    delete m_mgr;
    return wxApp::OnExit();
}

bool WatermarkingApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    STLImporter stlImp;
    world.uvsphere = stlImp.ReadFile("res/models/UVSphere.stl");
    world.cube = stlImp.ReadFile("res/models/cube.stl");

    m_project = std::make_shared<WatermarkingProject>();
    auto& window = ProjectWindow::Get(*m_project);
    auto page = window.GetMainPage();

    m_mgr = new wxAuiManager(page);

    auto& viewport = SceneViewportPane::Get(*m_project);
    auto outliner = new SceneOutlinerPane(page, *m_project);
    auto lattice = new LatticePane(page, *m_project);
    auto som = new SelfOrganizingMapPane(page, *m_project);
    auto watermarking = new WatermarkingPane(page, *m_project);
    watermarking->Disable();

    wxSize const minSize = page->FromDIP(wxSize(300, 30));

    m_mgr->AddPane(&viewport,
                   wxAuiPaneInfo()
                       .Name("viewport")
                       .Caption("3D Viewport")
                       .Center()
                       .CloseButton(true)
                       .MaximizeButton(true)
                       .MinSize(minSize));
    m_mgr->AddPane(outliner,
                   wxAuiPaneInfo()
                       .Name("outliner")
                       .Caption("Scene Outliner")
                       .Right()
                       .Layer(1)
                       .CloseButton(true)
                       .MaximizeButton(true)
                       .MinSize(minSize));
    m_mgr->AddPane(lattice,
                   wxAuiPaneInfo()
                       .Name("lattice")
                       .Caption("Lattice")
                       .Right()
                       .Layer(1)
                       .CloseButton(true)
                       .MaximizeButton(true)
                       .MinSize(minSize));
    m_mgr->AddPane(
        som,
        wxAuiPaneInfo().Name("som").Caption("SOM").Right().Layer(1).CloseButton(true).MaximizeButton(true).MinSize(
            minSize));
    m_mgr->AddPane(watermarking,
                   wxAuiPaneInfo()
                       .Name("watermarking")
                       .Caption("Watermarking")
                       .Right()
                       .Layer(1)
                       .CloseButton(true)
                       .MaximizeButton(true)
                       .MinSize(minSize)
                       .Hide());
    m_mgr->Update();

    window.Show();
    viewport.InitGL();

    // After OpenGL has been initialized
    Renderer::Get(*m_project);
    viewport.SetFocus();

    wxUpdateUIEvent::SetUpdateInterval(16);

    m_project->InitializeLattice();
    m_project->UpdateLatticeGraphics();

    window.Bind(wxEVT_MENU, &WatermarkingApp::OnTogglePane, this, EVT_VIEW_MENU_SCENE_VIEWPORT);
    window.Bind(wxEVT_MENU, &WatermarkingApp::OnTogglePane, this, EVT_VIEW_MENU_LATTICE);
    window.Bind(wxEVT_MENU, &WatermarkingApp::OnTogglePane, this, EVT_VIEW_MENU_SOM);
    window.Bind(wxEVT_MENU, &WatermarkingApp::OnTogglePane, this, EVT_VIEW_MENU_WATERMARKING);
    window.Bind(wxEVT_MENU, &WatermarkingApp::OnTogglePane, this, EVT_VIEW_MENU_SCENE_OUTLINER);

    return true;
}

void WatermarkingApp::ImportPolygonalModel(wxString const& path)
{
    if (path.EndsWith(".obj")) {
        OBJImporter objImp;
        world.theModel = std::make_shared<Mesh>(objImp.ReadFile(path.ToStdString()));
    } else if (path.EndsWith(".stl")) {
        STLImporter stlImp;
        world.theModel = std::make_shared<Mesh>(stlImp.ReadFile(path.ToStdString()));
    }

    m_project->SetDataset(std::make_shared<InputData>(world.theModel->positions));
    Renderer::Get(*m_project).LoadPolygonalModel(*world.theModel);
    Renderer::Get(*m_project).SetCameraView(CalculateBoundingBox(world.theModel->positions));
}

void WatermarkingApp::ImportVolumetricModel(wxString const& path)
{
    VolumeData data;

    world.theModel = std::make_shared<Mesh>();
    auto& pos = world.theModel->positions;
    auto& texcoord = world.theModel->textureCoords;

    if (!data.read(path.ToStdString())) {
        Logger::error(R"(Failed to read volumetric model: "%s")", path.ToStdString().c_str());
        return;
    }

    const int model = 252;
    const int isovalue = 0;
    auto reso = data.resolution();
    for (int x = 0; x < reso.x; x++) {
        for (int y = 0; y < reso.y; y++) {
            for (int z = 0; z < reso.z; z++) {
                if (data.value(x, y, z) >= model) {
                    if (x + 1 < reso.x) {
                        if (data.value(x + 1, y, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (x - 1 >= 0) {
                        if (data.value(x - 1, y, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (y + 1 < reso.y) {
                        if (data.value(x, y + 1, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (y - 1 >= 0) {
                        if (data.value(x, y - 1, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (z + 1 < reso.z) {
                        if (data.value(x, y, z + 1) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (z - 1 >= 0) {
                        if (data.value(x, y, z - 1) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                }
            }
        }
    }

    texcoord = std::vector<glm::vec2>(pos.size(), glm::vec2(0.0f, 0.0f));
    Logger::info("%lu voxels will be rendered.", pos.size());

    m_project->SetDataset(std::make_shared<InputData>(pos));
    Renderer::Get(*m_project).LoadVolumetricModel();
    Renderer::Get(*m_project).SetCameraView(CalculateBoundingBox(world.theModel->positions));
}

BoundingBox WatermarkingApp::CalculateBoundingBox(std::vector<glm::vec3> positions)
{
    const float FLOAT_MAX = std::numeric_limits<float>::max();
    glm::vec3 min = { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX };
    glm::vec3 max = { -FLOAT_MAX, -FLOAT_MAX, -FLOAT_MAX };

    for (auto const& p : positions) {
        if (p.x > max.x) {
            max.x = p.x;
        }
        if (p.y > max.y) {
            max.y = p.y;
        }
        if (p.z > max.z) {
            max.z = p.z;
        }
        if (p.x < min.x) {
            min.x = p.x;
        }
        if (p.y < min.y) {
            min.y = p.y;
        }
        if (p.z < min.z) {
            min.z = p.z;
        }
    }
    return { max, min };
}

void WatermarkingApp::OnToggleRenderOption(wxCommandEvent& event)
{
    RenderFlag opt = event.GetInt();
    world.renderFlags ^= opt;
}

void WatermarkingApp::OnToggleLatticeFlag(wxCommandEvent& event)
{
    LatticeFlags flags = Lattice::Get(*m_project).GetFlags() ^ event.GetInt();
    Lattice::Get(*m_project).SetFlags(flags);
}

void WatermarkingApp::OnMenuGenerateModel(wxCommandEvent& event)
{
    if (world.theModel == nullptr) {
        wxMessageDialog dialog(&ProjectWindow::Get(*m_project), "Please import a model from the File menu first.",
                               "Error", wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    Mesh mesh;
    BoundingBox bbox = CalculateBoundingBox(world.theModel->positions);

    if (event.GetId() == EVT_GENERATE_MODEL_DOME) {
        auto const& [max, min] = bbox;
        float const radius = glm::length((max - min) * 0.5f);
        int const numSegments = 60;
        int const numRings = 60;

        glm::vec3 const center = (max + min) * 0.5f;
        float deltaLong = glm::radians(360.0f) / numSegments;
        float deltaLat = glm::radians(180.0f) / numRings;

        std::vector<glm::vec3> temp;
        temp.resize((numRings - 1) * numSegments + 2);
        temp.front() = center + radius * glm::vec3(0.0f, cos(0.0f), 0.0f);
        temp.back() = center + radius * glm::vec3(0.0f, cos(glm::radians(180.0f)), 0.0f);

        // Iterate through the rings without the first and the last.
        for (int i = 1; i < numRings; i++) {
            for (int j = 0; j < numSegments; j++) {
                float const theta = deltaLat * i;
                float const phi = glm::radians(180.0f) - deltaLong * j;
                glm::vec3 const coord = radius * glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
                int const idx = (i - 1) * numSegments + j + 1;
                temp[idx] = center + coord;
            }
        }

        for (int j = 0; j < numSegments; j++) {
            auto const& p1 = temp[0];
            auto const& p2 = temp[(j + 1) % numSegments + 1];
            auto const& p3 = temp[j % numSegments + 1];
            glm::vec3 const normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
        }

        for (int i = 0; i < numRings - 2; i++) {
            for (int j = 0; j < numSegments; j++) {
                /**
                 *  4---3
                 *  |   |
                 *  1---2
                 */
                int k1 = (i * numSegments + (j % numSegments)) + 1;
                int k2 = (i * numSegments + ((j + 1) % numSegments)) + 1;
                int k3 = ((i + 1) * numSegments + ((j + 1) % numSegments)) + 1;
                int k4 = ((i + 1) * numSegments + (j % numSegments)) + 1;
                glm::vec3 p1, p2, p3, p4;
                glm::vec3 n1, n2;
                p1 = temp[k1]; // [x, y]
                p2 = temp[k2]; // [x + 1, y]
                p3 = temp[k3]; // [x + 1, y + 1]
                p4 = temp[k4]; // [x, y + 1]
                n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
                n2 = glm::normalize(glm::cross(p3 - p1, p4 - p1));

                mesh.positions.push_back(p1);
                mesh.positions.push_back(p2);
                mesh.positions.push_back(p3);
                mesh.positions.push_back(p1);
                mesh.positions.push_back(p3);
                mesh.positions.push_back(p4);
                mesh.normals.push_back(n1);
                mesh.normals.push_back(n1);
                mesh.normals.push_back(n1);
                mesh.normals.push_back(n2);
                mesh.normals.push_back(n2);
                mesh.normals.push_back(n2);
            }
        }
        for (int j = 0; j < numSegments; j++) {
            auto const& p1 = temp[((numRings - 2) * numSegments + 1) + (j % numSegments)];
            auto const& p2 = temp[((numRings - 2) * numSegments + 1) + ((j + 1) % numSegments)];
            auto const& p3 = temp.back();
            glm::vec3 const normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
            mesh.normals.push_back(normal);
        }
    }

    world.theModel = std::make_shared<Mesh>(mesh);
    m_project->SetDataset(std::make_shared<InputData>(mesh.positions));
    Renderer::Get(*m_project).LoadPolygonalModel(*world.theModel);
    Renderer::Get(*m_project).SetCameraView(CalculateBoundingBox(mesh.positions));
}

void WatermarkingApp::OnMenuImportModel(wxCommandEvent& event)
{
    wxString const filepath = event.GetString();
    if (filepath.EndsWith(".toml")) {
        ImportVolumetricModel(filepath);
    } else {
        ImportPolygonalModel(filepath);
    }
}

void WatermarkingApp::OnTogglePane(wxCommandEvent& event)
{
    wxString name;
    int const id = event.GetId();

    if (id == EVT_VIEW_MENU_LATTICE) {
        name = "lattice";
    } else if (id == EVT_VIEW_MENU_SOM) {
        name = "som";
    } else if (id == EVT_VIEW_MENU_WATERMARKING) {
        name = "watermarking";
    } else if (id == EVT_VIEW_MENU_SCENE_OUTLINER) {
        name = "outliner";
    } else if (id == EVT_VIEW_MENU_SCENE_VIEWPORT) {
        name = "viewport";
    }

    wxAuiPaneInfo& paneInfo = m_mgr->GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_mgr->Update();
}
