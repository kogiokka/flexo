#include <cmath>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Mesh.hpp"
#include "OpenGLCanvas.hpp"
#include "ProjectPanel.hpp"
#include "ProjectWindow.hpp"
#include "Renderer.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"

wxBEGIN_EVENT_TABLE(WatermarkingApp, wxApp)
    EVT_COMMAND(wxID_ANY, CMD_TOGGLE_RENDER_FLAG, WatermarkingApp::OnCmdToggleRenderOption)
    EVT_COMMAND(wxID_ANY, CMD_TOGGLE_LATTICE_FLAG, WatermarkingApp::OnCmdToggleLatticeFlag)
    EVT_COMMAND(wxID_ANY, CMD_REBUILD_LATTICE_MESH, WatermarkingApp::OnCmdRebuildLatticeMesh)
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
    auto& panel = ProjectPanel::Get(*m_project);
    auto& canvas = OpenGLCanvas::Get(*m_project);

    wxWindow* mainPage = window.GetMainPage();
    wxSizer* bs = mainPage->GetSizer();
    bs->Add(&panel, 1, wxGROW | wxALL, 0);
    bs->Add(&canvas, 3, wxGROW | wxALL, 0);
    bs->Layout();

    window.Show();
    canvas.InitGL();

    // After OpenGL has been initialized
    Renderer::Get(*m_project);
    canvas.SetFocus();

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

void WatermarkingApp::OnCmdToggleRenderOption(wxCommandEvent& evt)
{
    RenderFlag opt = evt.GetInt();
    world.renderFlags ^= opt;
}

void WatermarkingApp::OnCmdToggleLatticeFlag(wxCommandEvent& evt)
{
    LatticeFlags flags = Lattice::Get(*m_project).GetFlags() ^ evt.GetInt();
    Lattice::Get(*m_project).SetFlags(flags);
}

void WatermarkingApp::OnCmdRebuildLatticeMesh(wxCommandEvent&)
{
    m_project->BuildLatticeMesh();
}

void WatermarkingApp::OnMenuGenerateModel(wxCommandEvent& evt)
{
    if (world.theModel == nullptr) {
        Logger::info("You have to load a model into the scene first!");
        return;
    }

    Mesh mesh;
    BoundingBox bbox = CalculateBoundingBox(world.theModel->positions);

    if (evt.GetId() == EVT_GENERATE_MODEL_DOME) {
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

void WatermarkingApp::OnMenuImportModel(wxCommandEvent& evt)
{
    wxString const filepath = evt.GetString();
    if (filepath.EndsWith(".toml")) {
        ImportVolumetricModel(filepath);
    } else {
        ImportPolygonalModel(filepath);
    }
}

