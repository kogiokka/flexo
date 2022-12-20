#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>
#include <string>
#include <vector>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/weakref.h>

#include "Attachable.hpp"
#include "Mesh.hpp"
#include "SurfaceVoxels.hpp"
#include "gfx/drawable/DrawableBase.hpp"

using AttachedProjectObjects = HostBase<WatermarkingProject, AttachableBase, SharedPtr>;
using AttacheProjectWindows = HostBase<WatermarkingProject, wxWindow, BarePtr>;

class WatermarkingProject : public wxEvtHandler, public AttachedProjectObjects, public AttacheProjectWindows
{
public:
    using AttachedObjects = ::AttachedProjectObjects;
    using AttachedWindows = ::AttacheProjectWindows;

    WatermarkingProject();
    ~WatermarkingProject();
    void CreateScene();
    void CreateProject();
    void StopProject();
    void DoWatermark();
    void SetFrame(wxFrame* frame);
    void SetPanel(wxWindow* panel);
    wxWindow* GetPanel();
    void ImportVolumetricModel(wxString const& path);

private:
    void OnSOMPaneMapChanged(wxCommandEvent& event);
    void OnMenuAddModel(wxCommandEvent& event);

    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_panel;
    std::shared_ptr<SurfaceVoxels> m_model;
    std::string m_imageFile;
};

#endif
