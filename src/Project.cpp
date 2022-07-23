#include <glm/glm.hpp>

#include "Project.hpp"
#include "Renderer.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

wxDEFINE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

WatermarkingProject::WatermarkingProject()
    : m_isLatticeReady(false)
    , m_frame {}
    , m_panel {}
    , m_dataset(nullptr)
{
    Bind(EVT_PROJECT_SETTINGS_CHANGED, &WatermarkingProject::OnProjectSettingsChanged, this);
    Bind(EVT_LATTICE_INITIALIZED, &WatermarkingProject::OnLatticeInitialized, this);
    Bind(EVT_LATTICE_DIMENSIONS_CHANGED, &WatermarkingProject::OnLatticeDimensionsChanged, this);
}

void WatermarkingProject::Initialize()
{
    assert(m_dataset);
    SelfOrganizingMap::Get(*this).Initialize(m_dataset);
}

void WatermarkingProject::BuildLatticeMesh() const
{
    if (!m_isLatticeReady)
        return;

    Lattice const& lattice = Lattice::Get(*this);
    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = lattice.GetNeurons();
    positions.reserve(neurons.size());
    for (Node const& n : neurons) {
        positions.emplace_back(n[0], n[1], n[2]);
    }

    int const width = lattice.GetWidth();
    int const height = lattice.GetHeight();
    float const divisor = 1.1f; // FIXME

    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {

            unsigned int const idx = y * width + x;
            glm::vec3 p1, p2, p3, p4;
            glm::vec3 n1, n2;
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
            n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            n2 = glm::normalize(glm::cross(p3 - p1, p4 - p1));

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
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
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

void WatermarkingProject::UpdateLatticeEdges() const
{
    std::vector<unsigned int> indices;

    int const width = Lattice::Get(*this).GetWidth();
    int const height = Lattice::Get(*this).GetHeight();
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

void WatermarkingProject::SetDataset(std::shared_ptr<InputData> dataset)
{
    m_dataset = dataset;
}

void WatermarkingProject::SetFrame(wxFrame* frame)
{
    m_frame = frame;
}

void WatermarkingProject::SetPanel(wxWindow* panel)
{
    m_panel = panel;
}

void WatermarkingProject::DoWatermark()
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

void WatermarkingProject::OnLatticeInitialized(wxCommandEvent&)
{
    assert(m_dataset);
    SelfOrganizingMap::Get(*this).Initialize(m_dataset);

    // TODO: Better solution
    m_isLatticeReady = true;
    BuildLatticeMesh();

    Renderer::Get(*this).LoadLattice();
}

void WatermarkingProject::OnLatticeDimensionsChanged(wxCommandEvent&)
{
    Lattice::Get(*this).Initialize();
}

void WatermarkingProject::OnProjectSettingsChanged(wxCommandEvent&)
{
    auto& som = SelfOrganizingMap::Get(*this);
    auto const& settings = ProjectSettings::Get(*this);
    som.SetMaxIterations(settings.GetMaxIterations());
    som.SetLearningRate(settings.GetLearningRate());
    som.SetNeighborhood(settings.GetNeighborhood());
}
