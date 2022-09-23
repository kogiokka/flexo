#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>
#include <vector>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/weakref.h>

#include "Attachable.hpp"
#include "gfx/drawable/DrawableBase.hpp"

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
    void UpdateLatticeGraphics();

private:
    void UpdateLatticeEdges() const;
    void ImportPolygonalModel(wxString const& path);
    void ImportVolumetricModel(wxString const& path);
    void OnMenuAddPlate(wxCommandEvent& event);
    void OnMenuAddModel(wxCommandEvent& event);
    void OnMenuImportModel(wxCommandEvent& event);
    void SetModelDrawable(std::shared_ptr<DrawableBase> drawable);
    void SetLatticeDrawables(std::vector<std::shared_ptr<DrawableBase>>& drawables);

    bool m_isLatticeReady;
    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_panel;

    // FIXME This is a temporary solution
    std::vector<std::shared_ptr<DrawableBase>> m_drawables; // Store the shared pointers until the project exits.
};

#endif
