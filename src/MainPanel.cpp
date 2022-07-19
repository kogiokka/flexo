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
#include "MainPanel.hpp"
#include "Project.hpp"
#include "ProjectSettings.hpp"
#include "Renderer.hpp"
#include "World.hpp"

wxDEFINE_EVENT(CMD_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDEFINE_EVENT(CMD_TOGGLE_LATTICE_FLAG, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_LATTICE, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_SOM_PROCEDURE, wxCommandEvent);

MainPanel::MainPanel(wxWindow* parent, WatermarkingProject& project)
    : wxPanel(parent, wxID_ANY)
    , m_project(project)
{
    m_notebook = new wxNotebook(this, MAIN_PANEL_NOTEBOOK, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
    m_notebook->Disable();
    PopulateProjectPage();
    PopulateRenderingPage();

    auto layout = new wxBoxSizer(wxVERTICAL);
    layout->Add(m_notebook, 1, wxGROW | wxALL, 10);
    SetSizer(layout);

    m_updateTimer = new wxTimer(this, TIMER_UI_UPDATE);
    m_updateTimer->Start(16); // 16 ms (60 fps)
}

MainPanel::~MainPanel()
{
    m_updateTimer->Stop();
}

void MainPanel::PopulateProjectPage()
{
    wxPanel* page = new wxPanel(m_notebook, wxID_ANY);
    wxBoxSizer* const boxLayout = new wxBoxSizer(wxVERTICAL);

    // Lattice (Map)
    {
        wxPanel* panel = new wxPanel(page, wxID_ANY);
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);

        wxIntegerValidator<int> validDimen;
        validDimen.SetRange(1, 512);

        m_tcLatticeWidth = new wxTextCtrl(panel, TE_LATTICE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validDimen);
        m_tcLatticeHeight = new wxTextCtrl(panel, TE_LATTICE_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxTE_CENTER, validDimen);
        *m_tcLatticeWidth << Lattice::Get(m_project).GetWidth();
        *m_tcLatticeHeight << Lattice::Get(m_project).GetHeight();

        auto chkBox1 = new wxCheckBox(panel, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
        auto chkBox2 = new wxCheckBox(panel, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
        chkBox1->SetValue(false);
        chkBox2->SetValue(false);

        row1->Add(new wxStaticText(panel, wxID_ANY, "Lattice Dimensions: "), 1,
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeWidth, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeHeight, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox1, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox2, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row4->Add(new wxButton(panel, BTN_INITIALIZE_LATTICE, "Intialize"), 1,
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        layout->Add(row1, 0, wxGROW | wxALL, 5);
        layout->Add(row2, 0, wxGROW | wxALL, 5);
        layout->Add(row3, 0, wxGROW | wxALL, 5);
        layout->Add(row4, 0, wxGROW | wxALL, 5);
        panel->SetSizer(layout);
        boxLayout->Add(panel, 0, wxGROW | wxALL, 10);
        boxLayout->Add(new wxStaticLine(page), 0, wxGROW | wxALL, 10);

        panel->Enable();
        m_latticeConfigPanel = panel;
    }

    // SOM procedure
    {
        wxPanel* panel = new wxPanel(page, wxID_ANY);
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);
        wxIntegerValidator<int> validIterCap;
        wxFloatingPointValidator<float> validLearnRate(2, nullptr);
        wxFloatingPointValidator<float> validNeighborhood(2, nullptr);
        validIterCap.SetMin(0);
        validLearnRate.SetRange(0.0f, 1.0f);
        validNeighborhood.SetRange(0.0f, 10000.0f); // TODO: Adjust the range according to the lattice's dimensions
        m_tcMaxIterations = new wxTextCtrl(panel, TE_MAX_ITERATIONS, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxTE_CENTER, validIterCap);
        m_tcLearningRate = new wxTextCtrl(panel, TE_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validLearnRate);
        m_tcInitialNeighborhood = new wxTextCtrl(panel, TE_INITIAL_NEIGHBORHOOD, wxEmptyString, wxDefaultPosition,
                                                 wxDefaultSize, wxTE_CENTER, validNeighborhood);

        *m_tcMaxIterations << ProjectSettings::Get(m_project).GetMaxIterations();
        *m_tcLearningRate << ProjectSettings::Get(m_project).GetLearningRate();

        row1->Add(new wxStaticText(panel, wxID_ANY, "Max Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row1->Add(m_tcMaxIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(new wxStaticText(panel, wxID_ANY, "Learning Rate: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row2->Add(m_tcLearningRate, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(new wxStaticText(panel, wxID_ANY, "Neighborhood: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT,
                  5);
        row3->Add(m_tcInitialNeighborhood, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        layout->Add(row1, 0, wxGROW | wxALL, 5);
        layout->Add(row2, 0, wxGROW | wxALL, 5);
        layout->Add(row3, 0, wxGROW | wxALL, 5);
        panel->SetSizer(layout);

        m_btnCreateProject = new wxButton(page, BTN_CREATE_PROJECT, "Create");
        m_btnStopProject = new wxButton(page, BTN_STOP_PROJECT, "Stop");
        m_btnCreateProject->Disable();
        m_btnStopProject->Disable();
        row4->Add(m_btnCreateProject, 1, wxGROW | wxLEFT | wxRIGHT, 5);
        row4->Add(m_btnStopProject, 1, wxGROW | wxLEFT | wxRIGHT, 5);

        boxLayout->Add(panel, 0, wxGROW | wxALL, 10);
        boxLayout->Add(row4, 0, wxGROW | wxALL, 10);
        boxLayout->Add(new wxStaticLine(page), 0, wxGROW | wxALL, 10);

        panel->Disable();
        m_projectConfigPanel = panel;
    }

    {
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);
        m_btnPause = new wxButton(page, BTN_PLAY_PAUSE, "Pause");
        m_btnWatermark = new wxButton(page, BTN_WATERMARK, "Watermark");
        m_btnStart = new wxButton(page, BTN_START, "Start");
        m_btnPause->Disable();
        m_btnWatermark->Disable();
        m_btnStart->Disable();
        row1->Add(m_btnStart, 1, wxGROW | wxLEFT | wxRIGHT, 5);
        row2->Add(m_btnPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
        row2->Add(m_btnWatermark, 1, wxGROW | wxLEFT | wxRIGHT, 5);

        m_tcIterations
            = new wxTextCtrl(page, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
        m_tcIterations->SetCanFocus(false);
        m_tcNeighborhood
            = new wxTextCtrl(page, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
        m_tcNeighborhood->SetCanFocus(false);
        row3->Add(new wxStaticText(page, wxID_ANY, "Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(m_tcIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row4->Add(new wxStaticText(page, wxID_ANY, "Neighborhood: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row4->Add(m_tcNeighborhood, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
        boxLayout->Add(row4, 0, wxGROW | wxALL, 5);
        boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
        boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
    }

    page->SetSizer(boxLayout);
    m_notebook->AddPage(page, "Project");
}

void MainPanel::PopulateRenderingPage()
{
    wxPanel* page = new wxPanel(m_notebook, wxID_ANY);

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

    m_notebook->AddPage(page, "Rendering");
}

void MainPanel::OnButtonCreateProject(wxCommandEvent&)
{
    m_btnCreateProject->Disable();
    m_btnStopProject->Enable();

    m_btnStart->Enable();
    m_btnPause->Enable();

    m_project.Initialize();
}

void MainPanel::OnButtonStopProject(wxCommandEvent&)
{
    m_projectConfigPanel->Enable();
    m_btnCreateProject->Enable();
    m_btnStopProject->Disable();

    m_latticeConfigPanel->Enable();
    m_btnStart->Disable();
    m_btnPause->Disable();

    m_tcIterations->Clear();
    *m_tcIterations << 0;
    m_tcNeighborhood->Clear();
    *m_tcNeighborhood << 0.0f;

    m_project.Initialize();
}

void MainPanel::OnButtonInitializeLattice(wxCommandEvent&)
{
    Lattice::Get(m_project).Initialize();
    m_projectConfigPanel->Enable();
    m_btnCreateProject->Enable();
    m_btnStopProject->Disable();
}

void MainPanel::OnButtonStart(wxCommandEvent&)
{
    m_btnCreateProject->Disable();
    m_btnStopProject->Enable();
    m_latticeConfigPanel->Disable();
    m_projectConfigPanel->Disable();
    m_btnStart->Disable();
    m_btnPause->Enable();

    SelfOrganizingMap::Get(m_project).ToggleTraining();
}

void MainPanel::OnButtonPause(wxCommandEvent&)
{
    if (m_btnPause->GetLabel() == "Continue") {
        m_btnPause->SetLabel("Pause");
    } else {
        m_btnPause->SetLabel("Continue");
    }

    SelfOrganizingMap::Get(m_project).ToggleTraining();
}

void MainPanel::OnButtonWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
}

void MainPanel::OnCheckboxModel(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawModel);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeVertex);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeEdge);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLatticeFace(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeFace);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLightSource(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLightSource);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicX);
    ProcessWindowEvent(event);
}

void MainPanel::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    wxCommandEvent event(CMD_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicY);
    ProcessWindowEvent(event);
}

void MainPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}

void MainPanel::OnTimerUIUpdate(wxTimerEvent&)
{
    auto& som = SelfOrganizingMap::Get(m_project);

    m_tcIterations->Clear();
    *m_tcIterations << som.GetIterations();
    m_tcNeighborhood->Clear();
    *m_tcNeighborhood << som.GetNeighborhood();

    if (som.IsDone()) {
        m_btnWatermark->Enable();
    } else {
        m_btnWatermark->Disable();
    }
}

void MainPanel::OnUpdateUI(wxUpdateUIEvent& evt)
{
    switch (evt.GetId()) {
    case MAIN_PANEL_NOTEBOOK:
        if (world.theModel) {
            evt.Enable(true);
        }
        break;
    default:
        break;
    }
}

void MainPanel::OnSetLatticeWidth(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetWidth(tmp);
    }
}

void MainPanel::OnSetLatticeHeight(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetHeight(tmp);
    }
}

void MainPanel::OnSetMaxIterations(wxCommandEvent& evt)
{
    long tmp;
    if (evt.GetString().ToLong(&tmp)) {
        ProjectSettings::Get(m_project).SetMaxIterations(tmp);
    }
}

void MainPanel::OnSetLearningRate(wxCommandEvent& evt)
{
    double tmp;
    if (evt.GetString().ToDouble(&tmp)) {
        ProjectSettings::Get(m_project).SetLearningRate(tmp);
    }
}

void MainPanel::OnSetNeighborhood(wxCommandEvent& evt)
{
    double tmp;
    if (evt.GetString().ToDouble(&tmp)) {
        ProjectSettings::Get(m_project).SetNeighborhood(tmp);
    }
}

wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
    EVT_TEXT(TE_LATTICE_WIDTH, MainPanel::OnSetLatticeWidth)
    EVT_TEXT(TE_LATTICE_HEIGHT, MainPanel::OnSetLatticeHeight)
    EVT_TEXT(TE_MAX_ITERATIONS, MainPanel::OnSetMaxIterations)
    EVT_TEXT(TE_LEARNING_RATE, MainPanel::OnSetLearningRate)
    EVT_TEXT(TE_INITIAL_NEIGHBORHOOD, MainPanel::OnSetNeighborhood)
    EVT_BUTTON(BTN_START, MainPanel::OnButtonStart)
    EVT_BUTTON(BTN_PLAY_PAUSE, MainPanel::OnButtonPause)
    EVT_BUTTON(BTN_WATERMARK, MainPanel::OnButtonWatermark)
    EVT_BUTTON(BTN_INITIALIZE_LATTICE, MainPanel::OnButtonInitializeLattice)
    EVT_BUTTON(BTN_CREATE_PROJECT, MainPanel::OnButtonCreateProject)
    EVT_BUTTON(BTN_STOP_PROJECT, MainPanel::OnButtonStopProject)
    EVT_CHECKBOX(CB_RENDEROPT_MODEL, MainPanel::OnCheckboxModel)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, MainPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, MainPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, MainPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, MainPanel::OnSliderTransparency)
    EVT_TIMER(TIMER_UI_UPDATE, MainPanel::OnTimerUIUpdate)
    EVT_UPDATE_UI(MAIN_PANEL_NOTEBOOK, MainPanel::OnUpdateUI)
wxEND_EVENT_TABLE()
