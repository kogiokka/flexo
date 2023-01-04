#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <string>

#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/textctrl.h>

#include "object/ObjectList.hpp"
#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_PANE_OBJECT_CHANGED, wxCommandEvent);

struct TransformCtrl {
    struct LocationCtrl {
        wxTextCtrl* x;
        wxTextCtrl* y;
        wxTextCtrl* z;
    };
    struct RotationCtrl {
        wxTextCtrl* x;
        wxTextCtrl* y;
        wxTextCtrl* z;
    };
    struct ScaleCtrl {
        wxTextCtrl* x;
        wxTextCtrl* y;
        wxTextCtrl* z;
    };

    LocationCtrl location;
    RotationCtrl rotation;
    ScaleCtrl scale;
};

class FlexoProject;

class PropertiesPane : public ControlsPaneBase
{
public:
    PropertiesPane(wxWindow* parent, FlexoProject& project);

private:
    void OnObjectChanged(wxCommandEvent& event);
    void OnSelectDisplayType(wxCommandEvent& event);
    void OnCheckWireframe(wxCommandEvent& event);
    void GetObjectStatus(std::string const& id);
    void OnTransformLocation(wxCommandEvent& event);
    void OnTransformRotation(wxCommandEvent& event);
    void OnTransformScale(wxCommandEvent& event);

    bool m_hasWireframe;
    std::shared_ptr<Object> m_obj;
    wxCheckBox* m_chkWire;
    wxComboBox* m_combo;
    TransformCtrl m_transform;
    FlexoProject& m_project;
};

#endif
