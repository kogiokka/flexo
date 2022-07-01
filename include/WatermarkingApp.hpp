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
    std::shared_ptr<SelfOrganizingMap> const& GetSOM() const;
    void BuildLatticeMesh();
    void ImportPolygonalModel(wxString const& path);
    void ImportVolumetricModel(wxString const& path);
    void SetCameraView(std::vector<glm::vec3> positions);

    void OnSetLatticeWidth(wxCommandEvent& evt);
    void OnSetLatticeHeight(wxCommandEvent& evt);
    void OnSetMaxIterations(wxCommandEvent& evt);
    void OnSetLearningRate(wxCommandEvent& evt);
    void OnCmdStartTraining(wxCommandEvent& evt);
    void OnCmdStopTrainining(wxCommandEvent& evt);
    void OnCmdPauseTraining(wxCommandEvent& evt);
    void OnCmdToggleRenderOption(wxCommandEvent& evt);
    void OnCmdToggleLatticeFlag(wxCommandEvent& evt);
    void OnCmdDoWatermark(wxCommandEvent& evt);
    void OnCmdCreateLattice(wxCommandEvent& evt);
    void OnCmdCreateSOMProcedure(wxCommandEvent& evt);
    void OnCmdRebuildLatticeMesh(wxCommandEvent& evt);

private:
    void UpdateLatticeEdges();
    wxDECLARE_EVENT_TABLE();
};

#endif
