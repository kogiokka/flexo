#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

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
    TrainingConfig m_conf;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    TrainingConfig& GetTrainingConfig();
    void ToggleLatticeFlags(LatticeFlags flag);
    void CreateLattice();
};

#endif
