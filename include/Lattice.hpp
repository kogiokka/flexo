#ifndef LATTICE_H
#define LATTICE_H

#include <memory>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "InputData.hpp"
#include "Node.hpp"

using LatticeFlags = int;

enum LatticeFlags_ : int {
    LatticeFlags_CyclicNone = 1 << 0,
    LatticeFlags_CyclicX = 1 << 1,
    LatticeFlags_CyclicY = 1 << 2,
};

typedef enum {
    LatticeInitState_Plane,
    LatticeInitState_Random,
} LatticeInitState;

struct Lattice {
    int mWidth;
    int mHeight;
    LatticeFlags mFlags;
    std::vector<Node<3, 2>> mNeurons;
};

class WatermarkingProject;
using ListOfLattice = std::vector<std::shared_ptr<Lattice>>;

class LatticeList final : public AttachableBase, public ListOfLattice
{
    // privatize emplace_back and push_back method, use Add instead.
    using ListOfLattice::emplace_back;
    using ListOfLattice::push_back;

public:
    static LatticeList& Get(WatermarkingProject& project);
    static LatticeList const& Get(WatermarkingProject const& project);

    explicit LatticeList(WatermarkingProject& project);
    LatticeList(LatticeList const&) = delete;
    LatticeList& operator=(LatticeList const&) = delete;
    void Add(int width, int height, LatticeFlags flags,
             LatticeInitState initState = LatticeInitState_Random,
             BoundingBox box = { { -100.0f, -100.0f, -100.0f }, { 100.0f, 100.0f, 100.0f } });
    void SetCurrent(unsigned int index);
    std::shared_ptr<Lattice> GetCurrent() const;

private:
    WatermarkingProject& m_project;
    std::shared_ptr<Lattice> m_curr;
};

#endif
