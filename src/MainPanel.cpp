#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "MainPanel.hpp"
#include "World.hpp"

enum {
    TC_SET_ITERATION_CAP = wxID_HIGHEST + 1,
    TC_SET_LEARNING_RATE,
    TC_SET_LAT_WIDTH,
    TC_SET_LAT_HEIGHT,
    BTN_PLAY_PAUSE,
    BTN_WATERMARK,
    BTN_CONFIRM_AND_RESET,
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

void MainPanel::SetLattice(std::shared_ptr<Lattice> lattice)
{
    m_lattice = lattice;

    *m_tcIterCap << m_lattice->IterationCap();
    *m_tcInitLearningRate << m_lattice->InitialRate();
    *m_tcLatWidth << m_lattice->Width();
    *m_tcLatHeight << m_lattice->Height();
}

void MainPanel::SetupTraning()
{
    long tmpLong;
    double tmpDouble;
    if (m_tcLatWidth->GetValue().ToLong(&tmpLong)) {
        m_latticeWidth = tmpLong;
    }
    if (m_tcLatHeight->GetValue().ToLong(&tmpLong)) {
        m_latticeHeight = tmpLong;
    }
    if (m_tcIterCap->GetValue().ToLong(&tmpLong)) {
        m_iterationCap = tmpLong;
    }
    if (m_tcInitLearningRate->GetValue().ToDouble(&tmpDouble)) {
        m_initLearningRate = static_cast<float>(tmpDouble);
    }

    world.lattice = std::make_unique<Lattice>(m_latticeWidth, m_latticeHeight);
    world.lattice->SetTrainingParameters(m_initLearningRate, m_iterationCap, m_latFlags);
    world.lattice->Train(*world.dataset);
}

void MainPanel::SetOpenGLCanvas(OpenGLCanvas* canvas)
{
    m_canvas = canvas;
}

inline wxStaticBoxSizer* MainPanel::CreatePanelStaticBox1()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, this, "SOM Settings");
    auto const box = boxLayout->GetStaticBox();

    auto iterCapLabel = new wxStaticText(box, wxID_ANY, "Iteration Cap: ");
    auto learnRateLabel = new wxStaticText(box, wxID_ANY, "Learning Rate: ");
    auto dimenLabel = new wxStaticText(box, wxID_ANY, "Dimension: ");

    wxIntegerValidator<int> validIterCap;
    wxIntegerValidator<int> validDimen;
    wxFloatingPointValidator<float> validLearnRate(2, nullptr);
    validIterCap.SetMin(0);
    validDimen.SetRange(1, 512);
    validLearnRate.SetRange(0.0f, 1.0f);

    m_tcIterCap = new wxTextCtrl(box, TC_SET_ITERATION_CAP, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                 wxTE_CENTER, validIterCap);
    m_tcInitLearningRate = new wxTextCtrl(box, TC_SET_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validLearnRate);
    m_tcLatWidth = new wxTextCtrl(box, TC_SET_LAT_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                  validDimen);
    m_tcLatHeight = new wxTextCtrl(box, TC_SET_LAT_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                   validDimen);

    auto chkBox1 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
    auto chkBox2 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
    chkBox1->SetValue(m_latFlags & LatticeFlags_CyclicX);
    chkBox2->SetValue(m_latFlags & LatticeFlags_CyclicY);

    m_btnConfirm = new wxButton(box, BTN_CONFIRM_AND_RESET, "Confirm and Reset");
    m_btnConfirm->Disable();

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);
    auto row4 = new wxBoxSizer(wxHORIZONTAL);
    auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
    row1->Add(iterCapLabel, 1, sizerFlag, 5);
    row1->Add(m_tcIterCap, 1, sizerFlag, 5);
    row2->Add(learnRateLabel, 1, sizerFlag, 5);
    row2->Add(m_tcInitLearningRate, 1, sizerFlag, 5);
    row3->Add(dimenLabel, 1, sizerFlag, 5);
    row3->Add(m_tcLatWidth, 1, sizerFlag, 5);
    row3->Add(m_tcLatHeight, 1, sizerFlag, 5);
    row4->Add(chkBox1, 1, sizerFlag, 5);
    row4->Add(chkBox2, 1, sizerFlag, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row4, 0, wxGROW | wxALL, 5);
    boxLayout->Add(m_btnConfirm, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

inline wxStaticBoxSizer* MainPanel::CreatePanelStaticBox2()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, this, "SOM Control");
    auto const box = boxLayout->GetStaticBox();

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);

    m_btnPlayPause = new wxButton(box, BTN_PLAY_PAUSE, "Start");
    m_btnPlayPause->Disable();

    m_btnWatermark = new wxButton(box, BTN_WATERMARK, "Watermark");
    m_btnWatermark->Disable();

    auto currIterLabel = new wxStaticText(box, wxID_ANY, "Iterations: ");
    m_tcIterCurr = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    m_tcIterCurr->SetCanFocus(false);

    row1->Add(currIterLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row1->Add(m_tcIterCurr, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row2->Add(m_btnPlayPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    row3->Add(m_btnWatermark, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row3, 0, wxGROW | wxALL, 10);

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
    int const sliderInit = static_cast<int>(100.0f - m_canvas->GetModelTransparency() * 100.0f);
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
    assert(world.lattice);
    assert(world.volModel);

    // Update the texture coordinates of the Volumetric Model.
    std::vector<glm::vec2> textureCoords;
    textureCoords.reserve(world.volModel->textureCoords.size());

    for (glm::vec3 const& vp : world.volModel->positions) {
        glm::vec2 coord = world.latticeMesh.textureCoords.front();
        float minDist = glm::distance(vp, world.latticeMesh.positions.front());
        // TODO: Deal with the duplicate calculations
        for (unsigned int i = 1; i < world.latticeMesh.positions.size(); i++) {
            auto dist = glm::distance(vp, world.latticeMesh.positions[i]);
            if (dist < minDist) {
                minDist = dist;
                coord = world.latticeMesh.textureCoords[i];
            }
        }
        textureCoords.push_back(coord);
    }
    world.volModel->textureCoords = textureCoords;
    world.isWatermarked = true;
}

void MainPanel::OnButtonConfirmAndReset(wxCommandEvent&)
{
    assert(world.dataset != nullptr);
    SetupTraning();
    m_btnPlayPause->SetLabel("Start");
    m_canvas->ResetLattice();
}

void MainPanel::OnButtonPlayPause(wxCommandEvent&)
{
    assert(world.lattice != nullptr);
    world.lattice->ToggleTraining();
    if (m_btnPlayPause->GetLabel() == "Start") {
        m_btnPlayPause->SetLabel("Pause");
    } else {
        m_btnPlayPause->SetLabel("Start");
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
    m_latFlags ^= LatticeFlags_CyclicX;
}

void MainPanel::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    m_latFlags ^= LatticeFlags_CyclicY;
}

void MainPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}

void MainPanel::OnTimerUIUpdate(wxTimerEvent&)
{
    if (m_tcIterCurr != nullptr) {
        m_tcIterCurr->Clear();
        *m_tcIterCurr << world.lattice->CurrentIteration();

        if (world.lattice->IsTrainingDone()) {
            m_btnWatermark->Enable();
        } else {
            m_btnWatermark->Disable();
        }
    }
}

wxBEGIN_EVENT_TABLE(MainPanel, wxPanel)
    EVT_BUTTON(BTN_PLAY_PAUSE, MainPanel::OnButtonPlayPause)
    EVT_BUTTON(BTN_WATERMARK, MainPanel::OnButtonWatermark)
    EVT_BUTTON(BTN_CONFIRM_AND_RESET, MainPanel::OnButtonConfirmAndReset)
    EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainPanel::OnCheckboxInputDataset)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, MainPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, MainPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, MainPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, MainPanel::OnSliderTransparency)
    EVT_TIMER(TIMER_UI_UPDATE, MainPanel::OnTimerUIUpdate)
wxEND_EVENT_TABLE()
