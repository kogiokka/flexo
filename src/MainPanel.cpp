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

#include "MainPanel.hpp"
#include "Renderer.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"

wxDECLARE_APP(WatermarkingApp);

wxDEFINE_EVENT(CMD_START_TRAINING, wxCommandEvent);
wxDEFINE_EVENT(CMD_STOP_TRAINING, wxCommandEvent);
wxDEFINE_EVENT(CMD_PAUSE_TRAINING, wxCommandEvent);
wxDEFINE_EVENT(CMD_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDEFINE_EVENT(CMD_TOGGLE_LATTICE_FLAG, wxCommandEvent);
wxDEFINE_EVENT(CMD_DO_WATERMARK, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_LATTICE, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_SOM_PROCEDURE, wxCommandEvent);

MainPanel::MainPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
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

void MainPanel::SetOpenGLCanvas(OpenGLCanvas* canvas)
{
    m_canvas = canvas;
}

void MainPanel::PopulateProjectPage()
{
    wxPanel* page = new wxPanel(m_notebook, wxID_ANY);

    std::array<wxBoxSizer*, 9> rows;
    for (wxBoxSizer*& sizer : rows) {
        sizer = new wxBoxSizer(wxHORIZONTAL);
    }

    wxIntegerValidator<int> validIterCap;
    wxFloatingPointValidator<float> validLearnRate(2, nullptr);
    validIterCap.SetMin(0);
    validLearnRate.SetRange(0.0f, 1.0f);
    m_tcMaxIterations = new wxTextCtrl(page, TE_MAX_ITERATIONS, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxTE_CENTER, validIterCap);
    m_tcInitialRate = new wxTextCtrl(page, TE_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                     wxTE_CENTER, validLearnRate);

    *m_tcMaxIterations << 50000;
    *m_tcInitialRate << 0.15f;
    m_tcMaxIterations->SendTextUpdatedEvent();
    m_tcInitialRate->SendTextUpdatedEvent();
    m_btnConfirmSOM = new wxButton(page, BTN_CONFIRM_SOM, "Create Procedure");

    rows[0]->Add(new wxStaticText(page, wxID_ANY, "Max Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[0]->Add(m_tcMaxIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[1]->Add(new wxStaticText(page, wxID_ANY, "Learning Rate: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[1]->Add(m_tcInitialRate, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[2]->Add(m_btnConfirmSOM, 1, wxGROW | wxLEFT | wxRIGHT, 5);

    wxIntegerValidator<int> validDimen;
    validDimen.SetRange(1, 512);

    m_tcLatticeWidth = new wxTextCtrl(page, TE_LATTICE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxTE_CENTER, validDimen);
    m_tcLatticeHeight = new wxTextCtrl(page, TE_LATTICE_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxTE_CENTER, validDimen);
    *m_tcLatticeWidth << 64;
    *m_tcLatticeHeight << 64;
    m_tcLatticeWidth->SendTextUpdatedEvent();
    m_tcLatticeHeight->SendTextUpdatedEvent();
    auto chkBox1 = new wxCheckBox(page, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
    auto chkBox2 = new wxCheckBox(page, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
    chkBox1->SetValue(false);
    chkBox2->SetValue(false);
    m_btnConfirmLattice = new wxButton(page, BTN_CONFIRM_LATTICE, "Create Lattice");

    rows[3]->Add(new wxStaticText(page, wxID_ANY, "Dimension: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[3]->Add(m_tcLatticeWidth, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[3]->Add(m_tcLatticeHeight, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[4]->Add(chkBox1, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[4]->Add(chkBox2, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[5]->Add(m_btnConfirmLattice, 1, wxGROW | wxLEFT | wxRIGHT, 5);

    m_btnPlayPause = new wxButton(page, BTN_PLAY_PAUSE, "Continue");
    m_btnWatermark = new wxButton(page, BTN_WATERMARK, "Watermark");
    m_btnStart = new wxButton(page, BTN_START, "Start");
    m_btnStop = new wxButton(page, BTN_STOP, "Stop");
    m_btnPlayPause->Disable();
    m_btnWatermark->Disable();
    m_btnStart->Disable();
    m_btnStop->Disable();
    rows[6]->Add(m_btnStart, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    rows[6]->Add(m_btnStop, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    rows[7]->Add(m_btnPlayPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    rows[7]->Add(m_btnWatermark, 1, wxGROW | wxLEFT | wxRIGHT, 5);

    m_tcIterations = new wxTextCtrl(page, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    m_tcIterations->SetCanFocus(false);
    rows[8]->Add(new wxStaticText(page, wxID_ANY, "Iterations: "), 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    rows[8]->Add(m_tcIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

    wxBoxSizer* const boxLayout = new wxBoxSizer(wxVERTICAL);
    boxLayout->Add(rows[0], 0, wxGROW | wxALL, 5);
    boxLayout->Add(rows[1], 0, wxGROW | wxALL, 5);
    boxLayout->Add(rows[2], 0, wxGROW | wxALL, 5);
    boxLayout->Add(new wxStaticLine(page), 0, wxGROW | wxALL, 10);
    boxLayout->Add(rows[3], 0, wxGROW | wxALL, 5);
    boxLayout->Add(rows[4], 0, wxGROW | wxALL, 5);
    boxLayout->Add(rows[5], 0, wxGROW | wxALL, 5);
    boxLayout->Add(new wxStaticLine(page), 0, wxGROW | wxALL, 10);
    boxLayout->Add(rows[6], 0, wxGROW | wxALL, 5);
    boxLayout->Add(rows[7], 0, wxGROW | wxALL, 5);
    boxLayout->Add(new wxStaticLine(page), 0, wxGROW | wxALL, 10);
    boxLayout->Add(rows[8], 0, wxGROW | wxALL, 10);
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

void MainPanel::OnButtonWatermark(wxCommandEvent&)
{
    wxCommandEvent event(CMD_DO_WATERMARK, GetId());
    ProcessWindowEvent(event);
}

void MainPanel::OnButtonConfirmLattice(wxCommandEvent&)
{
    wxCommandEvent event(CMD_CREATE_LATTICE, GetId());
    ProcessWindowEvent(event);
}

void MainPanel::OnButtonConfirmSOM(wxCommandEvent&)
{
    wxCommandEvent event(CMD_CREATE_SOM_PROCEDURE, GetId());
    ProcessWindowEvent(event);

    m_btnPlayPause->SetLabel("Continue");
}

void MainPanel::OnButtonStart(wxCommandEvent&)
{
    if (!wxGetApp().GetSOM()) {
        return;
    }

    wxCommandEvent event(CMD_START_TRAINING, GetId());
    ProcessWindowEvent(event);

    m_btnStart->Disable();
    m_btnConfirmLattice->Disable();
    m_btnConfirmSOM->Disable();
    m_btnPlayPause->Enable();
}

void MainPanel::OnButtonStop(wxCommandEvent&)
{
    wxCommandEvent event(CMD_STOP_TRAINING, GetId());
    ProcessWindowEvent(event);

    m_tcIterations->Clear();
    *m_tcIterations << 0;
    m_btnStart->Enable();
    m_btnConfirmLattice->Enable();
    m_btnConfirmSOM->Enable();
    m_btnPlayPause->Disable();
}

void MainPanel::OnButtonPause(wxCommandEvent&)
{
    wxCommandEvent event(CMD_PAUSE_TRAINING, GetId());
    ProcessWindowEvent(event);

    if (m_btnPlayPause->GetLabel() == "Continue") {
        m_btnPlayPause->SetLabel("Pause");
    } else {
        m_btnPlayPause->SetLabel("Continue");
    }
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
    if (m_tcIterations == nullptr || wxGetApp().GetSOM() == nullptr) {
        return;
    }

    m_tcIterations->Clear();
    *m_tcIterations << wxGetApp().GetSOM()->GetIterations();

    if (wxGetApp().GetSOM()->IsTrainingDone()) {
        m_btnWatermark->Enable();
    } else {
        m_btnWatermark->Disable();
    }
}

void MainPanel::OnUpdateStartButton(wxUpdateUIEvent& evt)
{
    if (world.theModel) {
        evt.Enable(true);
    }
}

void MainPanel::OnUpdateStopButton(wxUpdateUIEvent& evt)
{
    if (world.theModel) {
        evt.Enable(true);
    }
}

wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
    EVT_BUTTON(BTN_START, MainPanel::OnButtonStart)
    EVT_BUTTON(BTN_STOP, MainPanel::OnButtonStop)
    EVT_BUTTON(BTN_PLAY_PAUSE, MainPanel::OnButtonPause)
    EVT_BUTTON(BTN_WATERMARK, MainPanel::OnButtonWatermark)
    EVT_BUTTON(BTN_CONFIRM_LATTICE, MainPanel::OnButtonConfirmLattice)
    EVT_BUTTON(BTN_CONFIRM_SOM, MainPanel::OnButtonConfirmSOM)
    EVT_CHECKBOX(CB_RENDEROPT_MODEL, MainPanel::OnCheckboxModel)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, MainPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, MainPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, MainPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, MainPanel::OnSliderTransparency)
    EVT_TIMER(TIMER_UI_UPDATE, MainPanel::OnTimerUIUpdate)
    EVT_UPDATE_UI(BTN_START, MainPanel::OnUpdateStartButton)
    EVT_UPDATE_UI(BTN_STOP, MainPanel::OnUpdateStopButton)
wxEND_EVENT_TABLE()
