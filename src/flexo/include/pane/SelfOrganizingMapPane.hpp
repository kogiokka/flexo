#ifndef SELF_ORGANIZING_MAP_PANE_H
#define SELF_ORGANIZING_MAP_PANE_H

#include <memory>

#include <wx/button.h>
#include <wx/textctrl.h>

#include "SelfOrganizingMap.hpp"
#include "SelfOrganizingMapModel.hpp"
#include "pane/ControlsPaneBase.hpp"

class SelfOrganizingMapPane : public ControlsPaneBase
{
public:
    SelfOrganizingMapPane(wxWindow* parent, FlexoProject& project);

private:
    void PopulateConfigPane();
    void PopulateTrainingPane();
    void OnConfigure(wxCommandEvent& event);
    void OnRun(wxCommandEvent& event);
    void OnParameterization(wxCommandEvent& event);

    wxButton* m_btnConfig;
    wxButton* m_btnRun;
    wxButton* m_btnTexmap;
    wxTextCtrl* m_textModel;
    wxTextCtrl* m_textMap;
    wxTextCtrl* m_textIter;
    wxTextCtrl* m_textRate;
    wxTextCtrl* m_textRadius;

    std::unique_ptr<SelfOrganizingMap> m_som;
    std::unique_ptr<SelfOrganizingMapModel<3, 2>> m_somModel;
};

#endif
