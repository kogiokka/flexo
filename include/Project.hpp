#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>

#include <wx/event.h>

#include "InputData.hpp"
#include "Lattice.hpp"
#include "ProjectSettings.hpp"
#include "Renderer.hpp"
#include "SelfOrganizingMap.hpp"

class WatermarkingProject : public wxEvtHandler
{
    friend ProjectSettings;

public:
    WatermarkingProject();
    void Create();
    void Stop();
    bool IsReady() const;
    bool IsTraining() const;
    bool IsDone() const;
    int GetIterations() const;
    float GetNeighborhood() const;
    void ToggleTraining();
    void BuildLatticeMesh() const;
    void DoWatermark();
    void SetDataset(std::shared_ptr<InputData> dataset);
    void OnProjectSettingsChanged(wxCommandEvent& evt);

private:
    ProjectSettings m_conf;
    std::shared_ptr<Lattice> m_lattice;
    std::shared_ptr<SelfOrganizingMap> m_som;
    std::shared_ptr<InputData> m_dataset;

    void UpdateLatticeEdges() const;
};

wxDECLARE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

#endif
