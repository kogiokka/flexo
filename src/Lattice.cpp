#include <memory>

#include "Lattice.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"

wxDEFINE_EVENT(EVT_LATTICE_DIMENSIONS_CHANGED, wxCommandEvent);

// Register factory: Lattice
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<Lattice> { return std::make_shared<Lattice>(project); }
};

void Notify(WatermarkingProject& project)
{
    wxCommandEvent event(EVT_LATTICE_DIMENSIONS_CHANGED);
    project.ProcessEvent(event);
}

Lattice& Lattice::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<Lattice>(factoryKey);
}

Lattice const& Lattice::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

Lattice::Lattice(WatermarkingProject& project)
    : m_project(project)
{
    m_width = 64;
    m_height = 64;
    m_flags = LatticeFlags_CyclicNone;
}

void Lattice::Initialize()
{
    RandomRealNumber<float> rng(-10.0f, 10.0f);

    m_neurons.clear();
    for (int j = 0; j < m_height; ++j) {
        for (int i = 0; i < m_width; ++i) {
            m_neurons.emplace_back(i, j, rng.vector(3));
        }
    }
}

void Lattice::SetWidth(int width)
{
    if (m_width == width) {
        return;
    }

    m_width = width;
    Notify(m_project);
}

void Lattice::SetHeight(int height)
{
    if (m_height == height) {
        return;
    }

    m_height = height;
    Notify(m_project);
}

void Lattice::SetFlags(LatticeFlags flags)
{
    m_flags = flags;
}

int Lattice::GetWidth() const
{
    return m_width;
}

int Lattice::GetHeight() const
{
    return m_height;
}

LatticeFlags Lattice::GetFlags() const
{
    return m_flags;
}

std::vector<Node>& Lattice::GetNeurons()
{
    return m_neurons;
}

std::vector<Node> const& Lattice::GetNeurons() const
{
    return m_neurons;
}
