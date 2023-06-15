#ifndef FLEXO_PROJECT_H
#define FLEXO_PROJECT_H

#include <wx/event.h>
#include <wx/frame.h>
#include <wx/weakref.h>

#include "Attachable.hpp"

class FlexoProject;

using AttachedProjectObjects = HostBase<FlexoProject, AttachableBase, SharedPtr>;
using AttacheProjectWindows = HostBase<FlexoProject, wxWindow, BarePtr>;

/**
 * A FlexoProject doesn't do anything itself. It holds all other classes'
 * instances as a whole project, and works as a bridge of events.
 */
class FlexoProject : public wxEvtHandler, public AttachedProjectObjects, public AttacheProjectWindows
{
public:
    using AttachedObjects = ::AttachedProjectObjects;
    using AttachedWindows = ::AttacheProjectWindows;

    FlexoProject();
    ~FlexoProject();
    void SetWindow(wxFrame* frame);
    wxFrame* GetWindow();

private:
    wxWeakRef<wxFrame> m_frame;
};

#endif
