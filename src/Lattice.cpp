#include <cassert>
#include <memory>
#include <utility>

#include "Lattice.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"

// Register factory: Lattice
static WatermarkingProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> SharedPtr<LatticeList> { return std::make_shared<LatticeList>(project); }
};

LatticeList& LatticeList::Get(WatermarkingProject& project)
{
    return project.AttachedObjects::Get<LatticeList>(factoryKey);
}

LatticeList const& LatticeList::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

LatticeList::LatticeList(WatermarkingProject& project)
    : m_project(project)
    , m_curr(nullptr)
{
}

void LatticeList::Add(int width, int height, LatticeFlags flags)
{
    RandomRealNumber<float> rng(-100.0f, 100.0f);

    auto lattice = std::make_shared<Lattice>();
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            lattice->mNeurons.emplace_back(i, j, rng.vector(3));
        }
    }

    lattice->mWidth = width;
    lattice->mHeight = height;
    lattice->mFlags = flags;

    emplace_back(std::move(lattice));
}

void LatticeList::SetCurrent(unsigned int index)
{
    if (index >= size()) {
        return;
    }
    m_curr = operator[](index);
}

std::shared_ptr<Lattice> LatticeList::GetCurrent() const
{
    return m_curr;
}
