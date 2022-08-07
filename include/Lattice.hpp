#ifndef LATTICE_H
#define LATTICE_H

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

class WatermarkingProject;

wxDECLARE_EVENT(EVT_LATTICE_DIMENSIONS_CHANGED, wxCommandEvent);

class Lattice : public AttachableBase
{
public:
    static Lattice& Get(WatermarkingProject& project);
    static Lattice const& Get(WatermarkingProject const& project);

    explicit Lattice(WatermarkingProject& project);
    Lattice(Lattice const&) = delete;
    Lattice& operator=(Lattice const&) = delete;
    void Initialize();
    void SetWidth(int width);
    void SetHeight(int height);
    void SetFlags(LatticeFlags flags);
    int GetWidth() const;
    int GetHeight() const;
    LatticeFlags GetFlags() const;
    std::vector<Node>& GetNeurons();
    std::vector<Node> const& GetNeurons() const;

private:
    WatermarkingProject& m_project;
    int m_width;
    int m_height;
    LatticeFlags m_flags;
    std::vector<Node> m_neurons;
};

#endif
