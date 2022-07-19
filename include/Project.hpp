#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>

#include <wx/event.h>

#include "Attachable.hpp"
#include "InputData.hpp"
#include "Lattice.hpp"
#include "ProjectSettings.hpp"
#include "Renderer.hpp"
#include "SelfOrganizingMap.hpp"

wxDECLARE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

using AttachedProjectObjects = HostBase<WatermarkingProject, AttachableBase>;

class WatermarkingProject : public wxEvtHandler, public AttachedProjectObjects
{
public:
    using AttachedObjects = ::AttachedProjectObjects;

    WatermarkingProject();
    void Create();
    void Stop();
    void BuildLatticeMesh() const;
    void DoWatermark();
    void SetDataset(std::shared_ptr<InputData> dataset);
    void OnLatticeInitialized(wxCommandEvent& evt);
    void OnLatticeDimensionsChanged(wxCommandEvent& evt);
    void OnProjectSettingsChanged(wxCommandEvent& evt);

private:
    bool m_isLatticeReady;
    std::shared_ptr<InputData> m_dataset;

    void UpdateLatticeEdges() const;
};

#endif
