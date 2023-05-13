#include "Project.hpp"
#include "pane/TransfromWidget.hpp"

wxDEFINE_EVENT(EVT_TRANSFORM_WIDGET_APPLY, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRANSFORM_WIDGET_LOCATION, Vec3Event);
wxDEFINE_EVENT(EVT_TRANSFORM_WIDGET_ROTATION, Vec3Event);
wxDEFINE_EVENT(EVT_TRANSFORM_WIDGET_SCALE, Vec3Event);

TransformWidget::TransformWidget(wxWindow* parent, FlexoProject& project)
    : ControlsGroup(parent, "Transform", 10)
    , m_project(project)
{
    m_location.x = AddInputText("Location X", "0");
    m_location.y = AddInputText("         Y", "0");
    m_location.z = AddInputText("         Z", "0");
    m_rotation.x = AddInputText("Rotoation X", "0");
    m_rotation.y = AddInputText("          Y", "0");
    m_rotation.z = AddInputText("          Z", "0");
    m_scale.x = AddInputText("Scale X", "1.0");
    m_scale.y = AddInputText("      Y", "1.0");
    m_scale.z = AddInputText("      Z", "1.0");

    m_location.x->Bind(wxEVT_TEXT, &TransformWidget::OnLocationChanged, this);
    m_location.y->Bind(wxEVT_TEXT, &TransformWidget::OnLocationChanged, this);
    m_location.z->Bind(wxEVT_TEXT, &TransformWidget::OnLocationChanged, this);
    m_rotation.x->Bind(wxEVT_TEXT, &TransformWidget::OnRotationChanged, this);
    m_rotation.y->Bind(wxEVT_TEXT, &TransformWidget::OnRotationChanged, this);
    m_rotation.z->Bind(wxEVT_TEXT, &TransformWidget::OnRotationChanged, this);
    m_scale.x->Bind(wxEVT_TEXT, &TransformWidget::OnScaleChanged, this);
    m_scale.y->Bind(wxEVT_TEXT, &TransformWidget::OnScaleChanged, this);
    m_scale.z->Bind(wxEVT_TEXT, &TransformWidget::OnScaleChanged, this);

    auto* btnApply = AddButton("Apply");
    btnApply->SetToolTip("Apply Object Transformation");
    btnApply->Bind(wxEVT_BUTTON, &TransformWidget::OnApply, this);
}

void TransformWidget::SetLocation(float x, float y, float z)
{
    m_location.x->Clear();
    m_location.y->Clear();
    m_location.z->Clear();
    *m_location.x << x;
    *m_location.y << y;
    *m_location.z << z;
}

void TransformWidget::SetRotation(float x, float y, float z)
{
    m_rotation.x->Clear();
    m_rotation.y->Clear();
    m_rotation.z->Clear();
    *m_rotation.x << x;
    *m_rotation.y << y;
    *m_rotation.z << z;
}

void TransformWidget::SetScale(float x, float y, float z)
{
    m_scale.x->Clear();
    m_scale.y->Clear();
    m_scale.z->Clear();
    *m_scale.x << x;
    *m_scale.y << y;
    *m_scale.z << z;
}

void TransformWidget::OnLocationChanged(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_location;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        Vec3Event event(EVT_TRANSFORM_WIDGET_LOCATION, this->GetId(), x, y, z);
        m_project.ProcessEvent(event);
    }
}

void TransformWidget::OnRotationChanged(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_rotation;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        Vec3Event event(EVT_TRANSFORM_WIDGET_ROTATION, this->GetId(), x, y, z);
        m_project.ProcessEvent(event);
    }
}

void TransformWidget::OnScaleChanged(wxCommandEvent&)
{
    double x, y, z;
    auto [xctrl, yctrl, zctrl] = m_scale;
    if (xctrl->GetValue().ToDouble(&x) && yctrl->GetValue().ToDouble(&y) && zctrl->GetValue().ToDouble(&z)) {
        Vec3Event event(EVT_TRANSFORM_WIDGET_SCALE, this->GetId(), x, y, z);
        m_project.ProcessEvent(event);
    }
}

void TransformWidget::OnApply(wxCommandEvent& event)
{
    event.SetEventType(EVT_TRANSFORM_WIDGET_APPLY);
    m_project.ProcessEvent(event);

    SetLocation(0.0f, 0.0f, 0.0f);
    SetRotation(0.0f, 0.0f, 0.0f);
    SetScale(1.0f, 1.0f, 1.0f);
}
