#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <string>

#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>
#include <wx/textctrl.h>

#include "event/ObjectSelectEvent.hpp"
#include "event/Vec3Event.hpp"
#include "object/ObjectList.hpp"
#include "pane/ControlsPaneBase.hpp"
#include "pane/TransfromWidget.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_PANE_OBJECT_SELECTED, ObjectSelectEvent);

class FlexoProject;

class PropertiesPane : public ControlsPaneBase
{
public:
    PropertiesPane(wxWindow* parent, FlexoProject& project);

private:
    void OnObjectSelected(ObjectSelectEvent& event);
    void OnSelectDisplayType(wxCommandEvent& event);
    void OnCheckWireframe(wxCommandEvent& event);
    void GetObjectStatus(std::string const& id);

    void OnTransformLocation(Vec3Event& event);
    void OnTransformRotation(Vec3Event& event);
    void OnTransformScale(Vec3Event& event);
    void OnTransformApply(Vec3Event& event);

    bool m_hasWireframe;
    std::shared_ptr<Object> m_obj;
    wxCheckBox* m_chkWire;
    wxComboBox* m_combo;
    TransformWidget* m_transform;
    FlexoProject& m_project;
};

#endif
