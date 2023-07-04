#include "dialog/SelfOrganizingMapDialog.hpp"
#include "Project.hpp"
#include "SceneController.hpp"
#include "SelfOrganizingMap.hpp"
#include "event/SliderFloatEvent.hpp"

#include <wx/artprov.h>
#include <wx/bmpcbox.h>
#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

SelfOrganizingMapDialog::SelfOrganizingMapDialog(wxWindow* parent, std::vector<std::string> mapIDs,
                                                 std::vector<std::string> objectIDs, FlexoProject& project)
    : wxDialog(parent, wxID_ANY, "Create SOM Project", wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_maxIterations(0)
    , m_leanringRate(0.0f)
    , m_neighborhood(0.0f)
    , m_project(project)
{
    wxTextCtrl *labelModel, *labelMap, *labelIter, *labelRate, *labelRadius;
    wxTextCtrl *textIter, *textRate;
    wxStaticText* textRadius;
    wxSlider* sliderRadius;
    wxBitmapComboBox *comboModel, *comboMap;
    wxStdDialogButtonSizer* stdbtn;

    comboModel = new wxBitmapComboBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);
    comboMap = new wxBitmapComboBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY);

    labelModel = new wxTextCtrl(this, wxID_ANY, "Model", wxDefaultPosition, wxDefaultSize,
                                wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    labelMap = new wxTextCtrl(this, wxID_ANY, "Map", wxDefaultPosition, wxDefaultSize,
                              wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    labelIter = new wxTextCtrl(this, wxID_ANY, "Max Iterations", wxDefaultPosition, wxDefaultSize,
                               wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    labelRate = new wxTextCtrl(this, wxID_ANY, "Learning Rate", wxDefaultPosition, wxDefaultSize,
                               wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);
    labelRadius = new wxTextCtrl(this, wxID_ANY, "Neighborhood", wxDefaultPosition, wxDefaultSize,
                                 wxBORDER_NONE | wxTE_RIGHT | wxTE_READONLY);

    textIter = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER);
    textRate = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_CENTER);
    sliderRadius = new wxSlider(this, wxID_ANY, 0, 0, 0);
    textRadius = new wxStaticText(this, wxID_ANY, wxString::Format("%.2f", 0.00f));

    stdbtn = CreateStdDialogButtonSizer(wxOK | wxCANCEL);

    // Set widgets properties
    auto bg = this->GetBackgroundColour();
    labelModel->SetBackgroundColour(bg);
    labelMap->SetBackgroundColour(bg);
    labelIter->SetBackgroundColour(bg);
    labelRate->SetBackgroundColour(bg);
    labelRadius->SetBackgroundColour(bg);
    labelModel->SetCanFocus(false);
    labelMap->SetCanFocus(false);
    labelIter->SetCanFocus(false);
    labelRate->SetCanFocus(false);
    labelRadius->SetCanFocus(false);

    // Default values
    m_maxIterations = 150000;
    m_leanringRate = 0.05f;
    *textIter << m_maxIterations;
    *textRate << m_leanringRate;

    for (auto id : mapIDs) {
        comboMap->Append(id, wxArtProvider::GetBitmap(wxART_WX_LOGO, wxART_OTHER, wxSize(16, 16)));
    }

    for (auto id : objectIDs) {
        comboModel->Append(id, wxArtProvider::GetBitmap(wxART_WX_LOGO, wxART_OTHER, wxSize(16, 16)));
    }

    wxIntegerValidator<int> validIter;
    wxFloatingPointValidator<float> validRate(6, nullptr);
    validIter.SetMin(0);
    validRate.SetRange(0.0f, 1.0f);
    textIter->SetValidator(validIter);
    textRate->SetValidator(validRate);

    // Binding
    textIter->Bind(wxEVT_TEXT, &SelfOrganizingMapDialog::OnMaxIterationChanged, this);
    textRate->Bind(wxEVT_TEXT, &SelfOrganizingMapDialog::OnInitialRateChanged, this);
    comboModel->Bind(wxEVT_COMBOBOX, &SelfOrganizingMapDialog::OnModelSelected, this);
    comboMap->Bind(wxEVT_COMBOBOX, &SelfOrganizingMapDialog::OnMapSelected, this);
    sliderRadius->Bind(wxEVT_SLIDER, &SelfOrganizingMapDialog::OnInitialNeighborhoodChanged, this);

    auto* btnOK = stdbtn->GetAffirmativeButton();
    btnOK->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) { event.Enable(m_map.lock() && m_object.lock()); });

    // Set members
    m_sliderRadius = sliderRadius;
    m_textRadius = textRadius;

    // Layout
    wxSizer* sizerSlider = new wxBoxSizer(wxHORIZONTAL);
    wxFlexGridSizer* grid = new wxFlexGridSizer(2, wxSize(10, 3));
    grid->AddGrowableCol(0, 4);
    grid->AddGrowableCol(1, 5);

    grid->Add(labelModel, wxSizerFlags().Expand().Proportion(4));
    grid->Add(comboModel, wxSizerFlags().Expand().Proportion(5));
    grid->Add(labelMap, wxSizerFlags().Expand().Proportion(4));
    grid->Add(comboMap, wxSizerFlags().Expand().Proportion(5));
    grid->Add(labelIter, wxSizerFlags().Expand().Proportion(4));
    grid->Add(textIter, wxSizerFlags().Expand().Proportion(5));
    grid->Add(labelRate, wxSizerFlags().Expand().Proportion(4));
    grid->Add(textRate, wxSizerFlags().Expand().Proportion(5));

    sizerSlider->Add(sliderRadius, wxSizerFlags().Expand().Proportion(4));
    sizerSlider->Add(textRadius, wxSizerFlags().Expand().Proportion(1));
    grid->Add(labelRadius, wxSizerFlags().Expand().Proportion(4));
    grid->Add(sizerSlider, wxSizerFlags().Expand().Proportion(5));

    m_topLayout = new wxBoxSizer(wxVERTICAL);
    m_topLayout->Add(grid, wxSizerFlags().Expand().Border(wxALL, 16));
    m_topLayout->Add(stdbtn, wxSizerFlags().Right().Border(wxALL, 16));
    m_topLayout->SetMinSize(GetSize());
    m_topLayout->SetSizeHints(this);

    SetAutoLayout(true);
    SetSizer(m_topLayout);
    CenterOnParent();
}

SelfOrganizingMapModel<3, 2> SelfOrganizingMapDialog::GetConfig() const
{
    SelfOrganizingMapModel<3, 2> model;
    model.map = m_map.lock();
    model.object = m_object.lock();
    model.learningRate = m_leanringRate;
    model.maxSteps = m_maxIterations;
    model.neighborhood = m_neighborhood;

    return model;
}

void SelfOrganizingMapDialog::OnModelSelected(wxCommandEvent& event)
{
    auto modelID = event.GetString().ToStdString();
    m_object = SceneController::Get(m_project).FindObject(modelID);
}

void SelfOrganizingMapDialog::OnMapSelected(wxCommandEvent& event)
{
    auto mapID = event.GetString().ToStdString();
    m_map = std::dynamic_pointer_cast<Map<3, 2>>(SceneController::Get(m_project).FindObject(mapID).lock());

    if (auto map = m_map.lock()) {
        int const width = map->size.x;
        int const height = map->size.y;
        float const diagLen = sqrt(width * width + height * height);
        float const radius = 0.5f * diagLen;
        m_neighborhood = radius;
        SetupNeighborhoodRadiusSlider(diagLen, radius);
    }
}

void SelfOrganizingMapDialog::OnMaxIterationChanged(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        m_maxIterations = tmp;
    }
}

void SelfOrganizingMapDialog::OnInitialRateChanged(wxCommandEvent& event)
{
    double tmp;
    if (event.GetString().ToDouble(&tmp)) {
        m_leanringRate = tmp;
    }
}

void SelfOrganizingMapDialog::OnInitialNeighborhoodChanged(wxCommandEvent& event)
{
    float const value = event.GetInt() * 0.01f;
    m_textRadius->SetLabelText(wxString::Format("%.2f", value));
    m_neighborhood = value;
}

void SelfOrganizingMapDialog::SetupNeighborhoodRadiusSlider(float maxValue, float value)
{
    float const scale = 100.0f;
    m_sliderRadius->SetMax(static_cast<int>(maxValue * scale));

    int const pos = static_cast<int>(value * scale);
    m_sliderRadius->SetValue(pos);

    wxCommandEvent sliderEvent(wxEVT_SLIDER);
    sliderEvent.SetInt(pos);
    m_sliderRadius->ProcessWindowEvent(sliderEvent);
}
