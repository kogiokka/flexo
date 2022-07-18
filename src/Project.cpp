#include <glm/glm.hpp>

#include "Project.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

wxDEFINE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

WatermarkingProject::WatermarkingProject()
    : m_lattice(nullptr)
    , m_som(nullptr)
    , m_dataset(nullptr)
{
    Bind(EVT_PROJECT_SETTINGS_CHANGED, &WatermarkingProject::OnProjectSettingsChanged, this);
}

void WatermarkingProject::Create()
{
    assert(m_dataset);

    int const width = ProjectSettings::Get(*this).GetLatticeWidth();
    int const height = ProjectSettings::Get(*this).GetLatticeHeight();

    m_lattice = std::make_shared<Lattice>(width, height);
    m_lattice->flags = ProjectSettings::Get(*this).GetLatticeFlags();
    BuildLatticeMesh();

    m_som = std::make_shared<SelfOrganizingMap>(ProjectSettings::Get(*this).GetLearningRate(),
                                                ProjectSettings::Get(*this).GetMaxIterations());
    m_som->Initialize(m_lattice, m_dataset);

    wxCommandEvent event(EVT_LATTICE_MESH_READY);
    ProcessEvent(event);
}

void WatermarkingProject::Stop()
{
    m_som = nullptr;
}

bool WatermarkingProject::IsReady() const
{
    return (m_som && m_lattice);
}

bool WatermarkingProject::IsTraining() const
{
    return (m_som != nullptr) && (m_som->IsTraining());
}

bool WatermarkingProject::IsDone() const
{
    return m_som->IsDone();
}

int WatermarkingProject::GetIterations() const
{
    return m_som->GetIterations();
}

float WatermarkingProject::GetNeighborhood() const
{
    return m_som->GetNeighborhood();
}

void WatermarkingProject::ToggleTraining()
{
    assert(m_som);
    m_som->ToggleTraining();
}

void WatermarkingProject::BuildLatticeMesh() const
{
    if (!m_lattice) {
        return;
    }

    Lattice const& lattice = *m_lattice;
    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = lattice.neurons;
    positions.reserve(neurons.size());
    for (Node const& n : neurons) {
        positions.emplace_back(n[0], n[1], n[2]);
    }

    int const width = ProjectSettings::Get(*this).GetLatticeWidth();
    int const height = ProjectSettings::Get(*this).GetLatticeHeight();
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

void WatermarkingProject::UpdateLatticeEdges() const
{
    std::vector<unsigned int> indices;

    int const width = ProjectSettings::Get(*this).GetLatticeWidth();
    int const height = ProjectSettings::Get(*this).GetLatticeHeight();
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

void WatermarkingProject::OnProjectSettingsChanged(wxCommandEvent& evt)
{
    // TODO: To guarantee the user instantiated them by creating the project.
    switch (static_cast<ProjectSettings::EventCode>(evt.GetInt())) {
    case ProjectSettings::EventCode_LatticeDimensionsChanged: {
        if (!m_lattice || !m_som) {
            return;
        }
        m_lattice = std::make_shared<Lattice>(ProjectSettings::Get(*this).GetLatticeWidth(),
                                              ProjectSettings::Get(*this).GetLatticeHeight());
        m_lattice->flags = ProjectSettings::Get(*this).GetLatticeFlags();
        m_som->Initialize(m_lattice, m_dataset);
        BuildLatticeMesh();
        wxCommandEvent e1(EVT_LATTICE_MESH_READY);
        ProcessEvent(e1);
    } break;
    case ProjectSettings::EventCode_ProcedureChanged:
        if (!m_som) {
            return;
        }
        m_som->SetMaxIterations(ProjectSettings::Get(*this).GetMaxIterations());
        m_som->SetLearningRate(ProjectSettings::Get(*this).GetLearningRate());
        m_som->SetNeighborhood(ProjectSettings::Get(*this).GetNeighborhood());
        break;
    }
}
