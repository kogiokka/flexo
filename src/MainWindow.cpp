#include <filesystem>
#include <string>
#include <vector>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/valnum.h>

#include "MainWindow.hpp"
#include "World.hpp"

enum {
    BTN_PLAY_PAUSE = wxID_HIGHEST + 1,
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
    TC_SET_ITERATION_CAP,
    TC_SET_LEARNING_RATE,
    TC_SET_LAT_WIDTH,
    TC_SET_LAT_HEIGHT,
    SLIDER_TRANSPARENCY,
    TIMER_UI_UPDATE,
};

MainWindow::MainWindow(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Self-organizing Map Demo", wxDefaultPosition, wxSize(1200, 800))
    , m_latFlags(LatticeFlags_CyclicNone)
    , m_timerUIUpdate(nullptr)
    , m_panel(nullptr)
    , m_tcIterCurr(nullptr)
    , m_tcLatWidth(nullptr)
    , m_tcLatHeight(nullptr)
    , m_tcIterCap(nullptr)
    , m_tcInitLearningRate(nullptr)
    , m_slider(nullptr)
    , m_canvas(nullptr)
{
    SetMinSize(wxSize(600, 400));
    Center();
    CreateStatusBar();

    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "Open Model");
    fileMenu->Append(wxID_EXIT, "Exit");

    auto cameraMenu = new wxMenu;
    cameraMenu->Append(wxID_REFRESH, "Reset");

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(cameraMenu, "&Camera");

    this->SetMenuBar(menubar);

    m_iterationCap = 50000;
    m_initLearningRate = 0.15f;

    float const ratio = static_cast<float>(world.pattern.width) / static_cast<float>(world.pattern.height);
    m_heightLat = 128;
    m_widthLat = static_cast<int>(m_heightLat * ratio);
    world.lattice = std::make_unique<Lattice>(m_widthLat, m_heightLat);
    CreateOpenGLCanvas();

    m_panel = new wxPanel(this, wxID_ANY);

    auto rootLayout = new wxBoxSizer(wxHORIZONTAL);
    // Proportion: Panel 1 to GLCanvas 3
    rootLayout->Add(m_panel, 1, wxGROW | wxALL, 0);
    rootLayout->Add(m_canvas, 3, wxGROW | wxALL, 0);
    this->SetSizer(rootLayout);
    this->Layout();

    auto panelLayout = new wxBoxSizer(wxVERTICAL);
    panelLayout->Add(CreatePanelStaticBox1(), 0, wxGROW | wxALL, 10);
    panelLayout->Add(CreatePanelStaticBox2(), 0, wxGROW | wxALL, 10);
    panelLayout->Add(CreatePanelStaticBox3(), 0, wxGROW | wxALL, 10);
    m_panel->SetSizer(panelLayout);

    m_timerUIUpdate = new wxTimer(this, TIMER_UI_UPDATE);
    m_timerUIUpdate->Start(16); // 16 ms (60 fps)
}

MainWindow::~MainWindow()
{
    // It seems that manually deleting widgets is unnecessary.
    // Using delete on wxPanel will cause some delay when the frame closes.
    m_timerUIUpdate->Stop();
    delete m_canvas;
    delete m_timerUIUpdate;
}

inline void MainWindow::CreateOpenGLCanvas()
{
    wxGLAttributes attrs;
    attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
    m_canvas = new OpenGLCanvas(this, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    m_canvas->SetFocus();
}

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox1()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, m_panel, "SOM Settings");
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

    m_tcIterCap = new wxTextCtrl(box, TC_SET_ITERATION_CAP, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                validIterCap);
    m_tcInitLearningRate = new wxTextCtrl(box, TC_SET_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                         wxTE_CENTER, validLearnRate);
    m_tcLatWidth = new wxTextCtrl(box, TC_SET_LAT_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                 validDimen);
    m_tcLatHeight = new wxTextCtrl(box, TC_SET_LAT_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                  validDimen);
    *m_tcIterCap << m_iterationCap;
    *m_tcInitLearningRate << m_initLearningRate;
    *m_tcLatWidth << m_widthLat;
    *m_tcLatHeight << m_heightLat;

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

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox2()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, m_panel, "SOM Control");
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

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox3()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, m_panel, "Rendering Options");
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

void MainWindow::InitializeGL()
{
    m_canvas->InitGL();
}

void MainWindow::OnTimerUIUpdate(wxTimerEvent&)
{
    if (m_tcIterCurr != nullptr) {
        assert(world.lattice != nullptr);
        m_tcIterCurr->Clear();
        *m_tcIterCurr << world.lattice->CurrentIteration();

        if (world.lattice->IsTrainingDone()) {
            m_btnWatermark->Enable();
        } else {
            m_btnWatermark->Disable();
        }
    }
    if (m_canvas != nullptr) {
        m_canvas->Refresh();
    }
}

void MainWindow::OnButtonPlayPause(wxCommandEvent&)
{
    assert(world.lattice != nullptr);
    world.lattice->ToggleTraining();
    if (m_btnPlayPause->GetLabel() == "Start") {
        m_btnPlayPause->SetLabel("Pause");
    } else {
        m_btnPlayPause->SetLabel("Start");
    }
}

void MainWindow::OnButtonWatermark(wxCommandEvent&)
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

void MainWindow::OnButtonConfirmAndReset(wxCommandEvent&)
{
    long tmpLong;
    double tmpDouble;
    if (m_tcLatWidth->GetValue().ToLong(&tmpLong)) {
        m_widthLat = tmpLong;
    } else {
        SetStatusText("Invalid lattice width!");
        return;
    }
    if (m_tcLatHeight->GetValue().ToLong(&tmpLong)) {
        m_heightLat = tmpLong;
    } else {
        SetStatusText("Invalid lattice height!");
        return;
    }
    if (m_tcIterCap->GetValue().ToLong(&tmpLong)) {
        m_iterationCap = tmpLong;
    } else {
        SetStatusText("Invalid lattice height!");
        return;
    }
    if (m_tcInitLearningRate->GetValue().ToDouble(&tmpDouble)) {
        m_initLearningRate = static_cast<float>(tmpDouble);
    } else {
        SetStatusText("Invalid initial learning rate!");
        return;
    }

    assert(world.dataset != nullptr);
    world.lattice = std::make_unique<Lattice>(m_widthLat, m_heightLat);
    world.lattice->Train(*world.dataset, m_initLearningRate, m_iterationCap, m_latFlags);
    m_btnPlayPause->SetLabel("Start");
    m_canvas->ResetLattice();
}

void MainWindow::OnCheckboxInputDataset(wxCommandEvent&)
{
    m_canvas->ToggleRenderOption(RenderOption_Model);
}

void MainWindow::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    m_canvas->ToggleRenderOption(RenderOption_LatticeVertex);
}

void MainWindow::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    m_canvas->ToggleRenderOption(RenderOption_LatticeEdge);
}

void MainWindow::OnCheckboxLatticeFace(wxCommandEvent&)
{
    m_canvas->ToggleRenderOption(RenderOption_LatticeFace);
}

void MainWindow::OnCheckboxLightSource(wxCommandEvent&)
{
    m_canvas->ToggleRenderOption(RenderOption_LightSource);
}

void MainWindow::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    m_latFlags ^= LatticeFlags_CyclicX;
}

void MainWindow::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    m_latFlags ^= LatticeFlags_CyclicY;
}

void MainWindow::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    float const alpha = (range - value) / range;
    m_canvas->SetModelColorAlpha(alpha);
}

void MainWindow::OnMenuCameraReset(wxCommandEvent&)
{
    m_canvas->ResetCamera();
}

void MainWindow::OnOpenFile(wxCommandEvent&)
{
    namespace fs = std::filesystem;
    static wxString defaultDir = "";

    wxFileDialog dialog(this, "Import Input Dataset", defaultDir, "",
                        "Volumetric models|*.toml|Polygonal models (*stl,*.obj)|*stl;*.obj",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    dialog.CenterOnParent();

    wxBusyCursor wait;
    if (dialog.ShowModal() == wxID_CANCEL) {
        defaultDir = fs::path(dialog.GetPath().ToStdString()).parent_path().string();
        return;
    }

    // Why wxBusyCursor does not work here?
    // wxBusyCursor wait;
    auto const filepath = dialog.GetPath().ToStdString();
    defaultDir = fs::path(filepath).parent_path().string();
    m_canvas->OpenInputDataFile(filepath);

    assert(world.dataset != nullptr);
    world.lattice->Train(*world.dataset, m_initLearningRate, m_iterationCap, m_latFlags);
    m_btnConfirm->Enable();
    m_btnPlayPause->Enable();
}

void MainWindow::OnExit([[maybe_unused]] wxCommandEvent& evt)
{
    Close(true);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(wxID_OPEN, MainWindow::OnOpenFile)
    EVT_MENU(wxID_EXIT, MainWindow::OnExit)
    EVT_MENU(wxID_REFRESH, MainWindow::OnMenuCameraReset)
    EVT_BUTTON(BTN_PLAY_PAUSE, MainWindow::OnButtonPlayPause)
    EVT_BUTTON(BTN_WATERMARK, MainWindow::OnButtonWatermark)
    EVT_BUTTON(BTN_CONFIRM_AND_RESET, MainWindow::OnButtonConfirmAndReset)
    EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainWindow::OnCheckboxInputDataset)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainWindow::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainWindow::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainWindow::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, MainWindow::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, MainWindow::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, MainWindow::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, MainWindow::OnSliderTransparency)
    EVT_TIMER(TIMER_UI_UPDATE, MainWindow::OnTimerUIUpdate)
wxEND_EVENT_TABLE()
