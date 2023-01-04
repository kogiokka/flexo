#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

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

class WatermarkingProject;
class SurfaceVoxels;

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
    void DoParameterization();
    void SetFrame(wxFrame* frame);
    void SetPanel(wxWindow* panel);
    wxWindow* GetPanel();
    void ImportVolumetricModel(wxString const& path);

private:
    void OnMenuAdd(wxCommandEvent& event);

    wxWeakRef<wxFrame> m_frame;
    wxWeakRef<wxWindow> m_panel;
    std::string m_imageFile;

public:
    std::shared_ptr<Dataset<3>> theDataset;
    std::weak_ptr<Map<3, 2>> theMap;
    std::weak_ptr<Object> theModel;
};

#endif
