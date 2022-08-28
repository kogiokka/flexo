#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/weakref.h>

#include "Attachable.hpp"
#include "Lattice.hpp"
#include "ProjectSettings.hpp"
#include "SelfOrganizingMap.hpp"

wxDECLARE_EVENT(EVT_LATTICE_MESH_READY, wxCommandEvent);

using AttachedProjectObjects = HostBase<WatermarkingProject, AttachableBase, SharedPtr>;
using AttacheProjectWindows = HostBase<WatermarkingProject, wxWindow, BarePtr>;

class WatermarkingProject : public wxEvtHandler, public AttachedProjectObjects, public AttacheProjectWindows
{
public:
    using AttachedObjects = ::AttachedProjectObjects;
    using AttachedWindows = ::AttacheProjectWindows;

    WatermarkingProject();
    void CreateProject();
    void StopProject();
    void BuildLatticeMesh() const;
    void DoWatermark();
    void SetFrame(wxFrame* frame);
    void SetPanel(wxWindow* panel);
    wxWindow* GetPanel();
    void OnLatticeDimensionsChanged(wxCommandEvent& event);
    void UpdateDatasetGraphics();
    void UpdateLatticeGraphics();

private:
    bool m_isLatticeReady;
    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_panel;

    void UpdateLatticeEdges() const;
};

#endif
