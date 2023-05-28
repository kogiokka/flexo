#ifndef SOM_PROJECT_DIALOG
#define SOM_PROJECT_DIALOG

#include <string>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

class FlexoProject;

class SelfOrganizingMapDialog : public wxDialog
{
public:
    SelfOrganizingMapDialog(wxWindow* parent, FlexoProject& project);
    std::string GetModelID() const;
    std::string GetMapID() const;

protected:
    void OnModelSelected(wxCommandEvent& event);
    void OnMapSelected(wxCommandEvent& event);
    void OnMaxIterationChanged(wxCommandEvent& event);
    void OnInitialRateChanged(wxCommandEvent& event);
    void OnInitialNeighborhoodChanged(wxCommandEvent& event);
    void SetupNeighborhoodRadiusSlider(float maxValue, float value);

    // SOM properties
    std::string m_modelID;
    std::string m_mapID;

    // Widgets
    wxSizer* m_topLayout;
    wxStaticText* m_textRadius;
    wxSlider* m_sliderRadius;
    FlexoProject& m_project;

    wxDECLARE_NO_COPY_CLASS(SelfOrganizingMapDialog);
};

#endif
