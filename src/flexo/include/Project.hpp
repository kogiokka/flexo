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

using AttachedProjectObjects = HostBase<FlexoProject, AttachableBase, SharedPtr>;
using AttacheProjectWindows = HostBase<FlexoProject, wxWindow, BarePtr>;

class FlexoProject : public wxEvtHandler, public AttachedProjectObjects, public AttacheProjectWindows
{
public:
    using AttachedObjects = ::AttachedProjectObjects;
    using AttachedWindows = ::AttacheProjectWindows;

    FlexoProject();
    ~FlexoProject();
    void SetWindow(wxFrame* frame);
    wxFrame* GetWindow();
    void ImportVolumetricModel(wxString const& path);

private:
    wxWeakRef<wxFrame> m_frame;
};

#endif
