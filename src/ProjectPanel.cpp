#include <array>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "Lattice.hpp"
#include "Project.hpp"
#include "ProjectPanel.hpp"
#include "ProjectSettings.hpp"
#include "ProjectWindow.hpp"
#include "Renderer.hpp"
#include "World.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDEFINE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

wxBEGIN_EVENT_TABLE(ProjectPanel, wxPanel)
    EVT_TEXT(TE_LATTICE_WIDTH, ProjectPanel::OnSetLatticeWidth)
    EVT_TEXT(TE_LATTICE_HEIGHT, ProjectPanel::OnSetLatticeHeight)
    EVT_TEXT(TE_MAX_ITERATIONS, ProjectPanel::OnSetMaxIterations)
    EVT_TEXT(TE_LEARNING_RATE, ProjectPanel::OnSetLearningRate)
    EVT_SLIDER(SLIDER_NEIGHBORHOOD_RADIUS, ProjectPanel::OnSliderNeighborhoodRadius)
    EVT_BUTTON(BTN_INITIALIZE_LATTICE, ProjectPanel::OnButtonInitializeLattice)
    EVT_BUTTON(BTN_RUN, ProjectPanel::OnButtonRun)
    EVT_BUTTON(BTN_WATERMARK, ProjectPanel::OnButtonWatermark)
    EVT_BUTTON(BTN_CREATE_PROJECT, ProjectPanel::OnButtonCreateProject)
    EVT_BUTTON(BTN_STOP_PROJECT, ProjectPanel::OnButtonStopProject)
    EVT_CHECKBOX(CB_RENDEROPT_MODEL, ProjectPanel::OnCheckboxModel)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, ProjectPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, ProjectPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, ProjectPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, ProjectPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, ProjectPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, ProjectPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, ProjectPanel::OnSliderTransparency)
    EVT_UPDATE_UI(BTN_RUN, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(PANEL_NOTEBOOK, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(PANEL_LATTICE, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(PANEL_SOM, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(PANEL_WATERMARKING, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(TE_ITERATIONS, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(TE_NEIGHBORHOOD, ProjectPanel::OnUpdateUI)
wxEND_EVENT_TABLE()

// Register factory: ProjectPanel
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        auto panel = new ProjectPanel(mainPage, PANEL_NOTEBOOK, wxDefaultPosition, wxDefaultSize, project);
        project.SetPanel(panel);
        return panel;
    }
};

ProjectPanel& ProjectPanel::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<ProjectPanel>(factoryKey);
}

ProjectPanel const& ProjectPanel::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

ProjectPanel::ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           WatermarkingProject& project)
    : wxNotebook(parent, id, pos, size)
    , m_project(project)
{
    PopulateProjectPage();
    PopulateRenderingPage();

    m_project.Bind(EVT_UI_UPDATE_INITIAL_NEIGHBORHOOD_RADIUS, &ProjectPanel::OnUpdateInitialNeighborhoodRadius, this);
    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_LATTICE);
    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_SOM);
    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_WATERMARKING);

    isLatticeInitialized = false;
    isProjectStopped = true;

    Disable();
}

ProjectPanel::~ProjectPanel()
{
}

void ProjectPanel::PopulateProjectPage()
{
    wxPanel* sidePane = new wxPanel(this, wxID_ANY);
    wxPanel* panel1 = new wxPanel(sidePane, PANEL_LATTICE);
    wxPanel* panel2 = new wxPanel(sidePane, PANEL_SOM);
    wxPanel* panel3 = new wxPanel(sidePane, PANEL_WATERMARKING);
    panel2->Disable();
    panel3->Disable();

    m_auiManager.SetManagedWindow(sidePane);

    // Lattice (Map)
    {
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);

        wxIntegerValidator<int> validDimen;
        validDimen.SetRange(1, 512);

        m_tcLatticeWidth = new wxTextCtrl(panel1, TE_LATTICE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validDimen);
        m_tcLatticeHeight = new wxTextCtrl(panel1, TE_LATTICE_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxTE_CENTER, validDimen);
        *m_tcLatticeWidth << Lattice::Get(m_project).GetWidth();
        *m_tcLatticeHeight << Lattice::Get(m_project).GetHeight();

        auto chkBox1 = new wxCheckBox(panel1, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
        auto chkBox2 = new wxCheckBox(panel1, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
        chkBox1->SetValue(false);
        chkBox2->SetValue(false);

        row1->Add(new wxStaticText(panel1, wxID_ANY, "Lattice Dimensions: "), 1,
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeWidth, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeHeight, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox1, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox2, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row4->Add(new wxButton(panel1, BTN_INITIALIZE_LATTICE, "Initialize"),
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        layout->Add(row1, 0, wxGROW | wxALL, 5);
        layout->Add(row2, 0, wxGROW | wxALL, 5);
        layout->Add(row3, 0, wxGROW | wxALL, 5);
        layout->Add(row4, 0, wxGROW | wxALL, 5);
        panel1->SetSizer(layout);
    }

    // SOM
    {
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);
        auto row5 = new wxBoxSizer(wxHORIZONTAL);
        auto row6 = new wxBoxSizer(wxHORIZONTAL);
        auto row7 = new wxBoxSizer(wxHORIZONTAL);
        auto row8 = new wxBoxSizer(wxHORIZONTAL);

        wxIntegerValidator<int> validMaxIter;
        wxFloatingPointValidator<float> validLearnRate(2, nullptr);
        validMaxIter.SetMin(0);
        validLearnRate.SetRange(0.0f, 1.0f);
        m_tcMaxIterations = new wxTextCtrl(panel2, TE_MAX_ITERATIONS, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxTE_CENTER, validMaxIter);
        m_tcLearningRate = new wxTextCtrl(panel2, TE_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validLearnRate);

        *m_tcMaxIterations << ProjectSettings::Get(m_project).GetMaxIterations();
        *m_tcLearningRate << ProjectSettings::Get(m_project).GetLearningRate();

        m_btnRun = new wxButton(panel2, BTN_RUN, "Pause");
        m_btnRun->Disable();

        m_tcIterations
            = new wxTextCtrl(panel2, TE_ITERATIONS, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
        m_tcIterations->SetCanFocus(false);
        m_tcNeighborhood = new wxTextCtrl(panel2, TE_NEIGHBORHOOD, "0", wxDefaultPosition, wxDefaultSize,
                                          wxTE_READONLY | wxTE_CENTER);
        m_tcNeighborhood->SetCanFocus(false);

        m_btnCreateProject = new wxButton(panel2, BTN_CREATE_PROJECT, "Create");
        m_btnStopProject = new wxButton(panel2, BTN_STOP_PROJECT, "Stop");
        m_btnStopProject->Disable();

        m_sliderRadius = new wxSlider(panel2, SLIDER_NEIGHBORHOOD_RADIUS, 0, 0, 0, wxDefaultPosition, wxDefaultSize);
        m_neighborhoodRadiusText = new wxStaticText(panel2, wxID_ANY, "0.0");

        row1->Add(new wxStaticText(panel2, wxID_ANY, "Max Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row1->Add(m_tcMaxIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(new wxStaticText(panel2, wxID_ANY, "Learning Rate: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row2->Add(m_tcLearningRate, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(new wxStaticText(panel2, wxID_ANY, "Neighborhood Radius: "), 1,
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(m_neighborhoodRadiusText, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row8->Add(m_sliderRadius, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        row4->Add(m_btnCreateProject, 1, wxGROW | wxALL, 3);
        row4->Add(m_btnStopProject, 1, wxGROW | wxALL, 3);
        row4->Add(m_btnRun, 1, wxGROW | wxALL, 3);

        row6->Add(new wxStaticText(panel2, wxID_ANY, "Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row6->Add(m_tcIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row7->Add(new wxStaticText(panel2, wxID_ANY, "Neighborhood: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row7->Add(m_tcNeighborhood, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        layout->Add(row1, 0, wxGROW | wxALL, 5);
        layout->Add(row2, 0, wxGROW | wxALL, 5);
        layout->Add(row3, 0, wxGROW | wxALL, 5);
        layout->Add(row8, 0, wxGROW | wxALL, 5);
        layout->Add(row4, 0, wxGROW | wxALL, 5);
        layout->Add(row5, 0, wxGROW | wxALL, 5);
        layout->Add(new wxStaticLine(panel2), 0, wxGROW | wxALL, 5);
        layout->Add(row6, 0, wxGROW | wxALL, 5);
        layout->Add(row7, 0, wxGROW | wxALL, 5);
        panel2->SetSizer(layout);
    }

    // Watermarking
    {
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        row1->Add(new wxButton(panel3, BTN_WATERMARK, "Watermark"), 1, wxGROW | wxLEFT | wxRIGHT, 5);
        layout->Add(row1, 0, wxGROW | wxALL, 5);
        panel3->SetSizer(layout);
    }

    wxAuiPaneInfo info = wxAuiPaneInfo().Center().CloseButton(false);
    info.dock_proportion = 2;
    m_auiManager.AddPane(panel1, info.Name("lattice").Caption("Lattice"));
    info.dock_proportion = 3;
    m_auiManager.AddPane(panel2, info.Name("som").Caption("SOM"));
    info.dock_proportion = 1;
    m_auiManager.AddPane(panel3, info.Name("watermark").Caption("Watermarking"));
    m_auiManager.Update();

    AddPage(sidePane, "Project");
}

void ProjectPanel::PopulateRenderingPage()
{
    wxPanel* page = new wxPanel(this, wxID_ANY);

    auto row1 = new wxBoxSizer(wxVERTICAL);
    auto row2 = new wxBoxSizer(wxVERTICAL);
    auto chkBox1 = new wxCheckBox(page, CB_RENDEROPT_MODEL, "Model");
    auto chkBox2 = new wxCheckBox(page, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
    auto chkBox3 = new wxCheckBox(page, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
    auto chkBox4 = new wxCheckBox(page, CB_RENDEROPT_LAT_FACE, "Lattice Face");
    auto chkBox5 = new wxCheckBox(page, CB_RENDEROPT_LIGHT_SOURCE, "Light Source");
    chkBox1->SetValue(true);
    chkBox2->SetValue(true);
    chkBox3->SetValue(true);
    chkBox4->SetValue(false);
    chkBox5->SetValue(false);

    // Trigger the event to update render flags.
    wxCommandEvent event;
    OnCheckboxModel(event);
    OnCheckboxLatticeVertex(event);
    OnCheckboxLatticeEdge(event);

    auto surfAlphaLabel = new wxStaticText(page, wxID_ANY, "Model Transparency (%)");
    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(page, SLIDER_TRANSPARENCY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS | wxSL_INVERSE);
    row1->Add(chkBox1, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox2, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox3, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox4, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox5, 0, wxGROW | wxALL, 5);
    row2->Add(surfAlphaLabel, 0, wxGROW);
    row2->Add(m_slider, 0, wxGROW);

    wxBoxSizer* boxLayout = new wxBoxSizer(wxVERTICAL);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);
    page->SetSizer(boxLayout);

    AddPage(page, "Rendering");
}

void ProjectPanel::OnButtonInitializeLattice(wxCommandEvent&)
{
    m_project.InitializeLattice();
    m_project.UpdateLatticeGraphics();
    isLatticeInitialized = true;
}

void ProjectPanel::OnButtonCreateProject(wxCommandEvent&)
{
    m_btnCreateProject->Disable();
    m_btnStopProject->Enable();
    m_btnRun->Enable();

    m_project.CreateProject();
    isProjectStopped = false;
}

void ProjectPanel::OnButtonStopProject(wxCommandEvent&)
{
    m_btnCreateProject->Enable();
    m_btnStopProject->Disable();
    m_btnRun->Disable();

    m_tcIterations->Clear();
    *m_tcIterations << 0;
    m_tcNeighborhood->Clear();
    *m_tcNeighborhood << 0.0f;

    m_project.StopProject();
    isProjectStopped = true;
}

void ProjectPanel::OnUpdateInitialNeighborhoodRadius(wxCommandEvent& event)
{
    float const radius = SelfOrganizingMap::Get(m_project).GetInitialNeighborhood();
    m_sliderRadius->SetValue(static_cast<int>(radius * 100.0f));
    m_neighborhoodRadiusText->SetLabelText(wxString::Format("%.3f", radius));

    // TODO: Get rid of the string indirection
    double maxRadius;
    if (event.GetString().ToDouble(&maxRadius)) {
        m_sliderRadius->SetMax(static_cast<int>(maxRadius * 100.0f));
    }
}

void ProjectPanel::OnButtonRun(wxCommandEvent&)
{
    m_btnCreateProject->Disable();
    m_btnStopProject->Enable();

    SelfOrganizingMap::Get(m_project).ToggleTraining();
}

void ProjectPanel::OnButtonWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}

void ProjectPanel::OnCheckboxModel(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawModel);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeVertex);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeEdge);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeFace(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeFace);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLightSource(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLightSource);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicX);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicY);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}

void ProjectPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    switch (event.GetId()) {
    case BTN_RUN:
        if (SelfOrganizingMap::Get(m_project).IsTraining()) {
            event.SetText("Pause");
        } else {
            event.SetText("Run");
        }
        break;
    case PANEL_NOTEBOOK:
        event.Enable(world.theModel != nullptr);
        break;
    case PANEL_LATTICE:
        event.Enable(isProjectStopped && !SelfOrganizingMap::Get(m_project).IsTraining());
        break;
    case PANEL_SOM:
        event.Enable(isLatticeInitialized);
        break;
    case PANEL_WATERMARKING:
        event.Enable(SelfOrganizingMap::Get(m_project).IsDone());
        break;
    case TE_ITERATIONS:
        m_tcIterations->Clear();
        *m_tcIterations << SelfOrganizingMap::Get(m_project).GetIterations();
        break;
    case TE_NEIGHBORHOOD:
        m_tcNeighborhood->Clear();
        *m_tcNeighborhood << SelfOrganizingMap::Get(m_project).GetNeighborhood();
        break;
    default:
        break;
    }
}

void ProjectPanel::OnSetLatticeWidth(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetWidth(tmp);
    }
}

void ProjectPanel::OnSetLatticeHeight(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetHeight(tmp);
    }
}

void ProjectPanel::OnSetMaxIterations(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        ProjectSettings::Get(m_project).SetMaxIterations(tmp);
    }
}

void ProjectPanel::OnSetLearningRate(wxCommandEvent& event)
{
    double tmp;
    if (event.GetString().ToDouble(&tmp)) {
        ProjectSettings::Get(m_project).SetLearningRate(tmp);
    }
}

void ProjectPanel::OnSliderNeighborhoodRadius(wxCommandEvent& event)
{
    float radius = static_cast<float>(event.GetInt()) / 100.0f;
    ProjectSettings::Get(m_project).SetNeighborhood(radius);
    m_neighborhoodRadiusText->SetLabelText(wxString::Format("%.3f", radius));
}

void ProjectPanel::OnTogglePane(wxCommandEvent& event)
{
    wxString name;
    int const id = event.GetId();

    if (id == EVT_VIEW_MENU_LATTICE) {
        name = "lattice";
    } else if (id == EVT_VIEW_MENU_SOM) {
        name = "som";
    } else if (id == EVT_VIEW_MENU_WATERMARKING) {
        name = "watermark";
    }

    wxAuiPaneInfo& paneInfo = m_auiManager.GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_auiManager.Update();
}
