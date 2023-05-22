#ifndef FLEXO_PROJECT_H
#define FLEXO_PROJECT_H

#include <memory>
#include <string>
#include <vector>

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/weakref.h>

#include "Attachable.hpp"
#include "Dataset.hpp"
#include "object/Map.hpp"
#include "object/Object.hpp"

class FlexoProject;
class SurfaceVoxels;

using AttachedProjectObjects = HostBase<FlexoProject, AttachableBase, SharedPtr>;
using AttacheProjectWindows = HostBase<FlexoProject, wxWindow, BarePtr>;

class FlexoProject : public wxEvtHandler, public AttachedProjectObjects, public AttacheProjectWindows
{
public:
    using AttachedObjects = ::AttachedProjectObjects;
    using AttachedWindows = ::AttacheProjectWindows;

    FlexoProject();
    ~FlexoProject();
    void CreateScene();
    void CreateProject();
    void StopProject();
    void DoParameterization();
    void SetFrame(wxFrame* frame);
    void SetPanel(wxWindow* panel);
    wxWindow* GetPanel();
    void ImportVolumetricModel(wxString const& path);

private:
    void OnMenuAdd(wxCommandEvent& event);

    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_panel;

public:
    std::shared_ptr<Dataset<3>> theDataset;
    std::weak_ptr<Map<3, 2>> theMap;
    std::weak_ptr<Object> theModel;
};

#endif
