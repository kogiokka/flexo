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
#include "WatermarkingApp.hpp"
#include "World.hpp"

wxDECLARE_APP(WatermarkingApp);

wxDEFINE_EVENT(CMD_START_TRAINING, wxCommandEvent);
wxDEFINE_EVENT(CMD_STOP_TRAINING, wxCommandEvent);
wxDEFINE_EVENT(CMD_DO_WATERMARK, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_LATTICE, wxCommandEvent);
wxDEFINE_EVENT(CMD_CREATE_SOM_PROCEDURE, wxCommandEvent);

enum {
    TE_MAX_ITERATIONS = wxID_HIGHEST + 1,
    TE_LEARNING_RATE,
    TE_LATTICE_WIDTH,
    TE_LATTICE_HEIGHT,
    BTN_PLAY_PAUSE,
    BTN_WATERMARK,
    BTN_CONFIRM_LATTICE,
    BTN_CONFIRM_SOM,
    BTN_START,
    BTN_STOP,
    SPCTRL_ITERATION_PER_FRAME,
    CB_RENDEROPT_SURFACE,
    CB_RENDEROPT_LAT_VERTEX,
    CB_RENDEROPT_LAT_EDGE,
    CB_RENDEROPT_LAT_FACE,
    CB_RENDEROPT_LIGHT_SOURCE,
    CB_LATTICE_FLAGS_CYCLIC_X,
    CB_LATTICE_FLAGS_CYCLIC_Y,
    SLIDER_TRANSPARENCY,
    TIMER_UI_UPDATE,
};

MainPanel::MainPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    auto layout = new wxBoxSizer(wxVERTICAL);
    layout->Add(CreatePanelStaticBox1(), 0, wxGROW | wxALL, 10);
    layout->Add(CreatePanelStaticBox2(), 0, wxGROW | wxALL, 10);
    layout->Add(CreatePanelStaticBox3(), 0, wxGROW | wxALL, 10);

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

inline wxStaticBoxSizer* MainPanel::CreatePanelStaticBox1()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, this, "Lattice");
    auto const box = boxLayout->GetStaticBox();

    auto dimenLabel = new wxStaticText(box, wxID_ANY, "Dimension: ");

    wxIntegerValidator<int> validDimen;
    validDimen.SetRange(1, 512);

    m_tcLatticeWidth = new wxTextCtrl(box, TE_LATTICE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                      wxTE_CENTER, validDimen);
    m_tcLatticeHeight = new wxTextCtrl(box, TE_LATTICE_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxTE_CENTER, validDimen);
    *m_tcLatticeWidth << wxGetApp().GetTrainingConfig().width;
    *m_tcLatticeHeight << wxGetApp().GetTrainingConfig().height;

    auto chkBox1 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
    auto chkBox2 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
    chkBox1->SetValue(wxGetApp().GetTrainingConfig().flags & LatticeFlags_CyclicX);
    chkBox2->SetValue(wxGetApp().GetTrainingConfig().flags & LatticeFlags_CyclicY);

    m_btnConfirmLattice = new wxButton(box, BTN_CONFIRM_LATTICE, "Create Lattice");

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
    row1->Add(dimenLabel, 1, sizerFlag, 5);
    row1->Add(m_tcLatticeWidth, 1, sizerFlag, 5);
    row1->Add(m_tcLatticeHeight, 1, sizerFlag, 5);
    row2->Add(chkBox1, 1, sizerFlag, 5);
    row2->Add(chkBox2, 1, sizerFlag, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
    boxLayout->Add(m_btnConfirmLattice, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

inline wxStaticBoxSizer* MainPanel::CreatePanelStaticBox2()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, this, "SOM Control");
    auto const box = boxLayout->GetStaticBox();

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);
    auto row4 = new wxBoxSizer(wxHORIZONTAL);
    auto row5 = new wxBoxSizer(wxHORIZONTAL);
    auto row6 = new wxBoxSizer(wxHORIZONTAL);

    wxIntegerValidator<int> validIterCap;
    wxFloatingPointValidator<float> validLearnRate(2, nullptr);
    validIterCap.SetMin(0);
    validLearnRate.SetRange(0.0f, 1.0f);
    auto maxIterLabel = new wxStaticText(box, wxID_ANY, "Max Iterations: ");
    auto learnRateLabel = new wxStaticText(box, wxID_ANY, "Learning Rate: ");
    m_tcMaxIterations = new wxTextCtrl(box, TE_MAX_ITERATIONS, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                       wxTE_CENTER, validIterCap);
    m_tcInitialRate = new wxTextCtrl(box, TE_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                     wxTE_CENTER, validLearnRate);

    *m_tcMaxIterations << wxGetApp().GetTrainingConfig().maxIterations;
    *m_tcInitialRate << wxGetApp().GetTrainingConfig().initialRate;

    m_btnPlayPause = new wxButton(box, BTN_PLAY_PAUSE, "Continue");
    m_btnPlayPause->Disable();
    m_btnConfirmSOM = new wxButton(box, BTN_CONFIRM_SOM, "Create Procedure");

    m_btnWatermark = new wxButton(box, BTN_WATERMARK, "Watermark");
    m_btnWatermark->Disable();

    auto iterLabel = new wxStaticText(box, wxID_ANY, "Iterations: ");
    m_tcIterations = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    m_tcIterations->SetCanFocus(false);

    m_btnStart = new wxButton(box, BTN_START, "Start");
    m_btnStop = new wxButton(box, BTN_STOP, "Stop");
    m_btnStart->Disable();
    m_btnStop->Disable();

    auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
    row1->Add(maxIterLabel, 1, sizerFlag, 5);
    row1->Add(m_tcMaxIterations, 1, sizerFlag, 5);
    row2->Add(learnRateLabel, 1, sizerFlag, 5);
    row2->Add(m_tcInitialRate, 1, sizerFlag, 5);
    row3->Add(m_btnConfirmSOM, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    auto line = new wxStaticLine(box);
    row4->Add(iterLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row4->Add(m_tcIterations, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row5->Add(m_btnStart, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    row5->Add(m_btnStop, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    row6->Add(m_btnPlayPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    row6->Add(m_btnWatermark, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
    boxLayout->Add(line, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row4, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row5, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row6, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

inline wxStaticBoxSizer* MainPanel::CreatePanelStaticBox3()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, this, "Rendering Options");
    auto const box = boxLayout->GetStaticBox();

    auto row1 = new wxBoxSizer(wxVERTICAL);
    auto row2 = new wxBoxSizer(wxVERTICAL);
    auto chkBox1 = new wxCheckBox(box, CB_RENDEROPT_SURFACE, "Model");
    auto chkBox2 = new wxCheckBox(box, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
    auto chkBox3 = new wxCheckBox(box, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
    auto chkBox4 = new wxCheckBox(box, CB_RENDEROPT_LAT_FACE, "Lattice Face");
    auto chkBox5 = new wxCheckBox(box, CB_RENDEROPT_LIGHT_SOURCE, "Light Source");
    chkBox1->SetValue(m_canvas->GetRenderOptionState(RenderOption_Model));
    chkBox2->SetValue(m_canvas->GetRenderOptionState(RenderOption_LatticeVertex));
    chkBox3->SetValue(m_canvas->GetRenderOptionState(RenderOption_LatticeEdge));
    chkBox4->SetValue(m_canvas->GetRenderOptionState(RenderOption_LatticeFace));
    chkBox5->SetValue(m_canvas->GetRenderOptionState(RenderOption_LightSource));
    auto surfAlphaLabel = new wxStaticText(box, wxID_ANY, "Model Transparency (%)");
    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(box, SLIDER_TRANSPARENCY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS | wxSL_INVERSE);
    row1->Add(chkBox1, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox2, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox3, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox4, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox5, 0, wxGROW | wxALL, 5);
    row2->Add(surfAlphaLabel, 0, wxGROW);
    row2->Add(m_slider, 0, wxGROW);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);

    return boxLayout;
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

    m_canvas->ResetLattice();
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

void MainPanel::OnButtonPlayPause(wxCommandEvent&)
{
    wxGetApp().ToggleTraining();
    if (m_btnPlayPause->GetLabel() == "Continue") {
        m_btnPlayPause->SetLabel("Pause");
    } else {
        m_btnPlayPause->SetLabel("Continue");
    }
}

void MainPanel::OnTextCtrlLatticeWidth(wxCommandEvent&)
{
    long tmp;
    if (m_tcLatticeWidth->GetValue().ToLong(&tmp)) {
        wxGetApp().GetTrainingConfig().width = tmp;
    }
}

void MainPanel::OnTextCtrlLatticeHeight(wxCommandEvent&)
{
    long tmp;
    if (m_tcLatticeHeight->GetValue().ToLong(&tmp)) {
        wxGetApp().GetTrainingConfig().height = tmp;
    }
}

void MainPanel::OnTextCtrlMaxIterations(wxCommandEvent&)
{
    long tmp;
    if (m_tcMaxIterations->GetValue().ToLong(&tmp)) {
        wxGetApp().GetTrainingConfig().maxIterations = tmp;
    }
}

void MainPanel::OnTextCtrlLearningRate(wxCommandEvent&)
{
    double tmp;
    if (m_tcInitialRate->GetValue().ToDouble(&tmp)) {
        wxGetApp().GetTrainingConfig().initialRate = tmp;
    }
}

void MainPanel::ToggleRenderOption(RenderOption opt)
{
    if (rendopt & opt) {
        rendopt -= opt;
    } else {
        rendopt += opt;
    }
}

void MainPanel::OnCheckboxInputDataset(wxCommandEvent&)
{
    ToggleRenderOption(RenderOption_Model);
}

void MainPanel::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    ToggleRenderOption(RenderOption_LatticeVertex);
}

void MainPanel::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    ToggleRenderOption(RenderOption_LatticeEdge);
}

void MainPanel::OnCheckboxLatticeFace(wxCommandEvent&)
{
    ToggleRenderOption(RenderOption_LatticeFace);
}

void MainPanel::OnCheckboxLightSource(wxCommandEvent&)
{
    ToggleRenderOption(RenderOption_LightSource);
}

void MainPanel::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    wxGetApp().ToggleLatticeFlags(LatticeFlags_CyclicX);
}

void MainPanel::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    wxGetApp().ToggleLatticeFlags(LatticeFlags_CyclicY);
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

wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
    EVT_BUTTON(BTN_START, MainPanel::OnButtonStart)
    EVT_BUTTON(BTN_STOP, MainPanel::OnButtonStop)
    EVT_BUTTON(BTN_PLAY_PAUSE, MainPanel::OnButtonPlayPause)
    EVT_BUTTON(BTN_WATERMARK, MainPanel::OnButtonWatermark)
    EVT_BUTTON(BTN_CONFIRM_LATTICE, MainPanel::OnButtonConfirmLattice)
    EVT_BUTTON(BTN_CONFIRM_SOM, MainPanel::OnButtonConfirmSOM)
    EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainPanel::OnCheckboxInputDataset)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, MainPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, MainPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, MainPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, MainPanel::OnSliderTransparency)
    EVT_TIMER(TIMER_UI_UPDATE, MainPanel::OnTimerUIUpdate)
    EVT_TEXT(TE_LATTICE_WIDTH, MainPanel::OnTextCtrlLatticeWidth)
    EVT_TEXT(TE_LATTICE_HEIGHT, MainPanel::OnTextCtrlLatticeHeight)
    EVT_TEXT(TE_MAX_ITERATIONS, MainPanel::OnTextCtrlMaxIterations)
    EVT_TEXT(TE_LEARNING_RATE, MainPanel::OnTextCtrlLearningRate)
wxEND_EVENT_TABLE()
