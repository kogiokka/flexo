#ifndef PROPERTIES_PANE_H
#define PROPERTIES_PANE_H

#include <string>

#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/event.h>

#include "gfx/ObjectList.hpp"
#include "pane/ControlsPaneBase.hpp"

wxDECLARE_EVENT(EVT_PROPERTIES_PANE_OBJECT_CHANGED, wxCommandEvent);

class WatermarkingProject;

class PropertiesPane : public ControlsPaneBase
{
public:
    PropertiesPane(wxWindow* parent, WatermarkingProject& project);

private:
    void OnObjectChanged(wxCommandEvent& event);
    void OnSelectDisplayType(wxCommandEvent& event);
    void OnCheckWireframe(wxCommandEvent& event);
    void GetObjectStatus(std::string const& id);

    bool m_hasWireframe;
    std::shared_ptr<Object> m_obj;
    wxCheckBox* m_chkWire;
    wxComboBox* m_combo;
    WatermarkingProject& m_project;
};

#endif
