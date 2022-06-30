#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "InputData.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "OpenGLCanvas.hpp"
#include "Renderer.hpp"
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
    std::shared_ptr<Renderer> m_renderer;
    std::shared_ptr<InputData> m_dataset;
    TrainingConfig m_conf;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    std::shared_ptr<Lattice> const& GetLattice() const;
    std::shared_ptr<SelfOrganizingMap> const& GetSOM() const;
    TrainingConfig& GetTrainingConfig();
    void ToggleLatticeFlags(LatticeFlags flag);
    void ToggleTraining();
    void DoWatermark();
    void CreateLattice();
    void CreateSOMProcedure();
    void StartTrainining();
    void StopTrainining();
    void BuildLatticeMesh();
    void UpdateLatticeEdges();
    void ImportPolygonalModel(wxString const& path);
    void ImportVolumetricModel(wxString const& path);
    void SetCameraView(std::vector<glm::vec3> positions);
};

#endif
