#include <array>
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

void LatticeList::Add(int width, int height, LatticeFlags flags, LatticeInitState initState, BoundingBox box)
{
    using std::array;
    auto lattice = std::make_shared<Lattice<3, 2>>();

    if (initState == LatticeInitState_Random) {
        RandomRealNumber<float> xRng(box.min.x, box.max.x);
        RandomRealNumber<float> yRng(box.min.y, box.max.y);
        RandomRealNumber<float> zRng(box.min.z, box.max.z);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                lattice->mNeurons.emplace_back(array<float, 3> { xRng.scalar(), yRng.scalar(), zRng.scalar() },
                                               array<float, 2> { static_cast<float>(i), static_cast<float>(j) });
            }
        }
    } else if (initState == LatticeInitState_Plane) {
        float dx = (box.max.x - box.min.x) / static_cast<float>(width);
        float dy = (box.max.y - box.min.y) / static_cast<float>(height);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                lattice->mNeurons.emplace_back(array<float, 3> { i * dx, j * dy, box.max.z },
                                               array<float, 2> { static_cast<float>(i), static_cast<float>(j) });
            }
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

std::shared_ptr<Lattice<3, 2>> LatticeList::GetCurrent() const
{
    return m_curr;
}
