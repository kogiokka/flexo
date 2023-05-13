#ifndef TRANSFORM_WIDGET
#define TRANSFORM_WIDGET

#include <wx/event.h>

#include "event/Vec3Event.hpp"
#include "pane/ControlsGroup.hpp"

wxDECLARE_EVENT(EVT_TRANSFORM_WIDGET_APPLY, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRANSFORM_WIDGET_LOCATION, Vec3Event);
wxDECLARE_EVENT(EVT_TRANSFORM_WIDGET_ROTATION, Vec3Event);
wxDECLARE_EVENT(EVT_TRANSFORM_WIDGET_SCALE, Vec3Event);

class FlexoProject;

class TransformWidget : public ControlsGroup
{
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

public:
    TransformWidget(wxWindow* parent, FlexoProject& project);
    void SetLocation(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);

private:
    void OnLocationChanged(wxCommandEvent& event);
    void OnRotationChanged(wxCommandEvent& event);
    void OnScaleChanged(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);

    LocationCtrl m_location;
    RotationCtrl m_rotation;
    ScaleCtrl m_scale;
    FlexoProject& m_project;
};

#endif
