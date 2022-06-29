#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <memory>

#include <wx/app.h>

#include "Lattice.hpp"
#include "SelfOrganizingMap.hpp"

struct TrainingConfig {
    int width;
    int height;
    float initialRate;
    int maxIterations;
    int flags;
};

class WatermarkingApp : public wxApp
{
    std::shared_ptr<SelfOrganizingMap> m_som;
    std::shared_ptr<Lattice> m_lattice;
    TrainingConfig m_conf;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    Lattice const& GetLattice() const;
    SelfOrganizingMap const& GetSOM() const;
    TrainingConfig& GetTrainingConfig();
    void ToggleLatticeFlags(LatticeFlags flag);
    void ToggleTraining();
    void DoWatermark();
    void CreateLattice();
    void BuildLatticeMesh();
    void UpdateLatticeEdges();
};

#endif
