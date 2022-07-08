#include <memory>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "MainPanel.hpp"
#include "MainWindow.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"

wxBEGIN_EVENT_TABLE(WatermarkingApp, wxApp)
    EVT_TEXT(TE_LATTICE_WIDTH, WatermarkingApp::OnSetLatticeWidth)
    EVT_TEXT(TE_LATTICE_HEIGHT, WatermarkingApp::OnSetLatticeHeight)
    EVT_TEXT(TE_MAX_ITERATIONS, WatermarkingApp::OnSetMaxIterations)
    EVT_TEXT(TE_LEARNING_RATE, WatermarkingApp::OnSetLearningRate)
    EVT_COMMAND(wxID_ANY, CMD_TOGGLE_RENDER_FLAG, WatermarkingApp::OnCmdToggleRenderOption)
    EVT_COMMAND(wxID_ANY, CMD_TOGGLE_LATTICE_FLAG, WatermarkingApp::OnCmdToggleLatticeFlag)
    EVT_COMMAND(wxID_ANY, CMD_START_TRAINING, WatermarkingApp::OnCmdStartTraining)
    EVT_COMMAND(wxID_ANY, CMD_STOP_TRAINING, WatermarkingApp::OnCmdStopTrainining)
    EVT_COMMAND(wxID_ANY, CMD_PAUSE_TRAINING, WatermarkingApp::OnCmdPauseTraining)
    EVT_COMMAND(wxID_ANY, CMD_DO_WATERMARK, WatermarkingApp::OnCmdDoWatermark)
    EVT_COMMAND(wxID_ANY, CMD_CREATE_LATTICE, WatermarkingApp::OnCmdCreateLattice)
    EVT_COMMAND(wxID_ANY, CMD_CREATE_SOM_PROCEDURE, WatermarkingApp::OnCmdCreateSOMProcedure)
    EVT_COMMAND(wxID_ANY, CMD_REBUILD_LATTICE_MESH, WatermarkingApp::OnCmdRebuildLatticeMesh)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(WatermarkingApp);

WatermarkingApp::WatermarkingApp()
    : m_som(nullptr)
    , m_lattice(nullptr)
    , m_renderer(nullptr)
    , m_dataset(nullptr)
    , m_conf {}
    , m_bbox {}
{
}

void WatermarkingApp::OnUnhandledException()
{
    throw;
}

int WatermarkingApp::OnExit()
{
    return wxApp::OnExit();
}

std::shared_ptr<SelfOrganizingMap> const& WatermarkingApp::GetSOM() const
{
    return m_som;
}
bool WatermarkingApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    STLImporter stlImp;
    world.uvsphere = stlImp.ReadFile("res/models/UVSphere.stl");
    world.cube = stlImp.ReadFile("res/models/cube.stl");

    MainWindow* window = new MainWindow();
    MainPanel* panel = new MainPanel(window);

    wxGLAttributes attrs;
    attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
    auto canvas = new OpenGLCanvas(window, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    canvas->SetFocus();

    panel->SetOpenGLCanvas(canvas);
    window->SetMainPanel(panel);
    window->SetOpenGLCanvas(canvas);

    window->Show();
    canvas->InitGL();

    wxSize const size = canvas->GetClientSize() * canvas->GetContentScaleFactor();
    m_renderer = std::make_shared<Renderer>(size.x, size.y);
    canvas->SetRenderer(m_renderer);

    return true;
}

void WatermarkingApp::BuildLatticeMesh()
{
    if (!m_lattice) {
        return;
    }

    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = m_lattice->neurons;
    positions.reserve(neurons.size());
    for (Node const& n : neurons) {
        positions.emplace_back(n[0], n[1], n[2]);
    }

    int const width = m_lattice->width;
    int const height = m_lattice->height;
    float const divisor = 1.1f; // FIXME

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {

            unsigned int const idx = y * width + x;
            glm::vec3 p1, p2, p3, p4;
            glm::vec3 n1, n2, n3, n4;
            glm::vec2 t1, t2, t3, t4;
            Face f1, f2;

            /**
             *  4-----3
             *  |     |
             *  |     |
             *  1-----2
             */
            p1 = positions[idx]; // [x, y]
            p2 = positions[idx + 1]; // [x + 1, y]
            p3 = positions[idx + width + 1]; // [x + 1, y + 1]
            p4 = positions[idx + width]; // [x, y + 1]

            // Normals
            n2 = glm::normalize(glm::cross(p2 - p1, p3 - p2));
            n4 = glm::normalize(glm::cross(p3 - p1, p4 - p3));

            if (isnan(n1.x) || isnan(n1.y) || isnan(n1.z)) {
                n1 = glm::vec3(0.0f, 0.0f, 0.0f);
            }
            if (isnan(n2.x) || isnan(n2.y) || isnan(n2.z)) {
                n2 = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            n3 = (n2 + n4) * 0.5f;
            n1 = (n2 + n4) * 0.5f;

            // TextureCoords
            t1 = glm::vec2(x / divisor, y / divisor);
            t2 = glm::vec2((x + 1) / divisor, y / divisor);
            t3 = glm::vec2((x + 1) / divisor, (y + 1) / divisor);
            t4 = glm::vec2(x / divisor, (y + 1) / divisor);

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p1);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p4);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n3);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n3);
            mesh.normals.push_back(n4);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t2);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t4);
        }
    }

    world.neurons.positions = positions;
    world.latticeMesh = mesh;

    // FIXME?
    UpdateLatticeEdges();
}

void WatermarkingApp::UpdateLatticeEdges()
{
    std::vector<unsigned int> indices;

    int const width = m_lattice->width;
    int const height = m_lattice->height;
    for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {
            indices.push_back(i * width + j);
            indices.push_back(i * width + j + 1);
            indices.push_back(i * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j);
            indices.push_back((i + 1) * width + j);
            indices.push_back(i * width + j);
        }
    }

    world.latticeEdges = indices;
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

    m_dataset = std::make_shared<InputData>(world.theModel->positions);
    m_renderer->LoadPolygonalModel(*world.theModel);
    m_bbox = CalculateBoundingBox(world.theModel->positions);
    SetCameraView(m_bbox);
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

    m_dataset = std::make_shared<InputData>(pos);
    m_renderer->LoadVolumetricModel();
    m_bbox = CalculateBoundingBox(world.theModel->positions);
    SetCameraView(m_bbox);
}

WatermarkingApp::BoundingBox WatermarkingApp::CalculateBoundingBox(std::vector<glm::vec3> positions)
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

// FIXME: Move it to the renderer
void WatermarkingApp::SetCameraView(BoundingBox box)
{
    auto const& [max, min] = box;
    glm::vec3 center = (max + min) * 0.5f;
    m_renderer->GetCamera().center = center;
    Logger::info("Camera looking at: %s", glm::to_string(center).c_str());

    m_renderer->GetCamera().UpdateViewCoord();
    m_renderer->LoadLattice();

    glm::vec3 diff = max - min;
    float size = diff.x;
    if (size < diff.y) {
        size = diff.y;
    }
    if (size < diff.z) {
        size = diff.z;
    }
    m_renderer->GetCamera().volumeSize = size;
}

void WatermarkingApp::OnSetLatticeWidth(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        m_conf.width = tmp;
    }
}

void WatermarkingApp::OnSetLatticeHeight(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        m_conf.height = tmp;
    }
}

void WatermarkingApp::OnSetMaxIterations(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        m_conf.maxIterations = tmp;
    }
}

void WatermarkingApp::OnSetLearningRate(wxCommandEvent& evt)
{
    double tmp;
    if (evt.GetString().ToDouble(&tmp)) {
        m_conf.initialRate = tmp;
    }
}

void WatermarkingApp::OnCmdStartTraining(wxCommandEvent&)
{
    m_som->Train(m_lattice, m_dataset);
}

void WatermarkingApp::OnCmdStopTrainining(wxCommandEvent&)
{
    m_som = nullptr;
}

void WatermarkingApp::OnCmdPauseTraining(wxCommandEvent&)
{
    if (!m_som) {
        return;
    }

    m_som->ToggleTraining();
}

void WatermarkingApp::OnCmdToggleRenderOption(wxCommandEvent& evt)
{
    RenderFlag opt = evt.GetInt();
    world.renderFlags ^= opt;
}

void WatermarkingApp::OnCmdToggleLatticeFlag(wxCommandEvent& evt)
{
    LatticeFlags flag = evt.GetInt();
    m_conf.flags ^= flag;
}

void WatermarkingApp::OnCmdDoWatermark(wxCommandEvent&)
{
    assert(world.theModel);

    // Update the texture coordinates of the Volumetric Model.
    std::vector<glm::vec2> textureCoords;
    textureCoords.reserve(world.theModel->textureCoords.size());

    for (glm::vec3 const& vp : world.theModel->positions) {
        glm::vec2 coord = world.latticeMesh.textureCoords.front();
        float minDist = glm::distance(vp, world.latticeMesh.positions.front());
        // TODO: Deal with the duplicate calculations
        for (unsigned int i = 1; i < world.latticeMesh.positions.size(); i++) {
            auto dist = glm::distance(vp, world.latticeMesh.positions[i]);
            if (dist < minDist) {
                minDist = dist;
                coord = world.latticeMesh.textureCoords[i];
            }
        }
        textureCoords.push_back(coord);
    }
    world.theModel->textureCoords = textureCoords;
    world.isWatermarked = true;
}

void WatermarkingApp::OnCmdCreateLattice(wxCommandEvent&)
{
    m_lattice = std::make_shared<Lattice>(m_conf.width, m_conf.height);
    m_lattice->flags = m_conf.flags;
    BuildLatticeMesh();

    m_renderer->LoadLattice(); // Change OpenGL vertex buffer's size
}

void WatermarkingApp::OnCmdCreateSOMProcedure(wxCommandEvent&)
{
    m_som = std::make_shared<SelfOrganizingMap>(m_conf.initialRate, m_conf.maxIterations);
}

void WatermarkingApp::OnCmdRebuildLatticeMesh(wxCommandEvent&)
{
    BuildLatticeMesh();
}
