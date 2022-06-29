#include <memory>

#include "MainPanel.hpp"
#include "MainWindow.hpp"
#include "OpenGLCanvas.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"
#include "assetlib/STL/STLImporter.hpp"

WatermarkingApp::WatermarkingApp()
{
    float const ratio = static_cast<float>(world.pattern.width) / static_cast<float>(world.pattern.height);
    m_conf.height = 128;
    m_conf.width = static_cast<int>(m_conf.height * ratio);
    m_conf.maxIterations = 50000;
    m_conf.initialRate = 0.15f;
}

void WatermarkingApp::OnUnhandledException()
{
    throw;
}

int WatermarkingApp::OnExit()
{
    return wxApp::OnExit();
}

Lattice const& WatermarkingApp::GetLattice() const
{
    return *m_lattice;
}

SelfOrganizingMap const& WatermarkingApp::GetSOM() const
{
    return *m_som;
}

TrainingConfig& WatermarkingApp::GetTrainingConfig()
{
    return m_conf;
}

void WatermarkingApp::ToggleLatticeFlags(LatticeFlags flag)
{
    m_conf.flags ^= flag;
}

void WatermarkingApp::ToggleTraining()
{
    m_som->ToggleTraining();
}

void WatermarkingApp::DoWatermark()
{
    assert(world.volModel);

    // Update the texture coordinates of the Volumetric Model.
    std::vector<glm::vec2> textureCoords;
    textureCoords.reserve(world.volModel->textureCoords.size());

    for (glm::vec3 const& vp : world.volModel->positions) {
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
    world.volModel->textureCoords = textureCoords;
    world.isWatermarked = true;
}

void WatermarkingApp::CreateLattice()
{
    m_lattice = std::make_shared<Lattice>(m_conf.width, m_conf.height);
    m_lattice->flags  = m_conf.flags;
    m_som = std::make_unique<SelfOrganizingMap>(m_conf.initialRate, m_conf.maxIterations);
    m_som->Train(m_lattice, *world.dataset);
}

bool WatermarkingApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    // FIXME
    m_lattice = std::make_shared<Lattice>(m_conf.width, m_conf.height);
    m_lattice->flags  = m_conf.flags;
    m_som = std::make_unique<SelfOrganizingMap>(m_conf.initialRate, m_conf.maxIterations);

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

    return true;
}

void WatermarkingApp::BuildLatticeMesh()
{
    if (m_som->IsTrainingDone()) {
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

wxIMPLEMENT_APP(WatermarkingApp);
