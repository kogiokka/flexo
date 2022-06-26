#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <memory>

#include <wx/app.h>

#include "Lattice.hpp"

struct TrainingConfig {
    int width;
    int height;
    float initialRate;
    int maxIterations;
    int flags;
};

class WatermarkingApp : public wxApp
{
    std::unique_ptr<Lattice> m_lattice;
    TrainingConfig m_conf;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    Lattice const& GetLattice();
    TrainingConfig& GetTrainingConfig();
    void ToggleLatticeFlags(LatticeFlags flag);
    void ToggleTraining();
    void CreateLattice();
    void BuildLatticeMesh();
    void UpdateLatticeEdges();
};

#endif
