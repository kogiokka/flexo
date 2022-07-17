#include <glm/glm.hpp>

#include "Project.hpp"
#include "World.hpp"

WatermarkingProject::WatermarkingProject()
    : m_conf(*this)
{
}

void WatermarkingProject::Train()
{
    int const width = m_conf.GetLatticeWidth();
    int const height = m_conf.GetLatticeHeight();
    m_lattice = std::make_shared<Lattice>(width, height);
    m_lattice->flags = m_conf.GetLatticeFlags();

    // float neighborhood = 0.5f * sqrt(width * width + height * height);
    m_som = std::make_shared<SelfOrganizingMap>(m_conf.GetLearningRate(), m_conf.GetMaxIterations(),
                                                m_conf.GetNeighborhood());
}

void WatermarkingProject::Stop()
{
    m_som = nullptr;
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
    m_som->ToggleTraining();
}

void WatermarkingProject::BuildLatticeMesh() const
{
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
    ;
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
