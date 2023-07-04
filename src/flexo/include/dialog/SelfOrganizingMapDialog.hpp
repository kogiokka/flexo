#ifndef SOM_PROJECT_DIALOG
#define SOM_PROJECT_DIALOG

#include <memory>
#include <vector>

#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "SelfOrganizingMap.hpp"
#include "object/Map.hpp"
#include "object/Object.hpp"

class FlexoProject;

class SelfOrganizingMapDialog : public wxDialog
{
public:
    SelfOrganizingMapDialog(wxWindow* parent, std::vector<std::string> mapIDs, std::vector<std::string> objectIDs,
                            FlexoProject& project);
    SelfOrganizingMapModel<3, 2> GetConfig() const;

protected:
    void OnModelSelected(wxCommandEvent& event);
    void OnMapSelected(wxCommandEvent& event);
    void OnMaxIterationChanged(wxCommandEvent& event);
    void OnInitialRateChanged(wxCommandEvent& event);
    void OnInitialNeighborhoodChanged(wxCommandEvent& event);
    void SetupNeighborhoodRadiusSlider(float maxValue, float value);

    // SOM properties
    std::weak_ptr<Map<3, 2>> m_map;
    std::weak_ptr<Object> m_object;
    long int m_maxIterations;
    float m_leanringRate;
    float m_neighborhood;

    // Widgets
    wxSizer* m_topLayout;
    wxStaticText* m_textRadius;
    wxSlider* m_sliderRadius;
    FlexoProject& m_project;

    wxDECLARE_NO_COPY_CLASS(SelfOrganizingMapDialog);
};

#endif
