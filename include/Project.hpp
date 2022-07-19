#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>

#include <wx/event.h>

#include "Attachable.hpp"
#include "InputData.hpp"
#include "Lattice.hpp"
#include "MainPanel.hpp"
#include "ProjectSettings.hpp"
#include "SelfOrganizingMap.hpp"

wxDECLARE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

using AttachedProjectObjects = HostBase<WatermarkingProject, AttachableBase>;

class WatermarkingProject : public wxEvtHandler, public AttachedProjectObjects
{
public:
    using AttachedObjects = ::AttachedProjectObjects;

    WatermarkingProject();
    void Initialize();
    void BuildLatticeMesh() const;
    void DoWatermark();
    void SetDataset(std::shared_ptr<InputData> dataset);
    void SetMainPanel(MainPanel* panel);
    void OnLatticeInitialized(wxCommandEvent& evt);
    void OnSOMInitialized(wxCommandEvent& evt);
    void OnLatticeDimensionsChanged(wxCommandEvent& evt);
    void OnProjectSettingsChanged(wxCommandEvent& evt);

private:
    bool m_isLatticeReady;
    MainPanel* m_panel;
    std::shared_ptr<InputData> m_dataset;

    void UpdateLatticeEdges() const;
};

#endif
