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
    , latFlags_(LatticeFlags_CyclicNone)
    , timerUIUpdate_(nullptr)
    , panel_(nullptr)
    , tcIterCurr_(nullptr)
    , tcLatWidth_(nullptr)
    , tcLatHeight_(nullptr)
    , tcIterCap_(nullptr)
    , tcInitLearningRate_(nullptr)
    , slider_(nullptr)
    , canvas_(nullptr)
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

    iterationCap_ = 50000;
    initLearningRate_ = 0.15f;

    float const ratio = static_cast<float>(world.pattern.width) / static_cast<float>(world.pattern.height);
    heightLat_ = 128;
    widthLat_ = static_cast<int>(heightLat_ * ratio);
    world.lattice = std::make_unique<Lattice>(widthLat_, heightLat_);
    CreateOpenGLCanvas();

    panel_ = new wxPanel(this, wxID_ANY);

    auto rootLayout = new wxBoxSizer(wxHORIZONTAL);
    // Proportion: Panel 1 to GLCanvas 3
    rootLayout->Add(panel_, 1, wxGROW | wxALL, 0);
    rootLayout->Add(canvas_, 3, wxGROW | wxALL, 0);
    this->SetSizer(rootLayout);
    this->Layout();

    auto panelLayout = new wxBoxSizer(wxVERTICAL);
    panelLayout->Add(CreatePanelStaticBox1(), 0, wxGROW | wxALL, 10);
    panelLayout->Add(CreatePanelStaticBox2(), 0, wxGROW | wxALL, 10);
    panelLayout->Add(CreatePanelStaticBox3(), 0, wxGROW | wxALL, 10);
    panel_->SetSizer(panelLayout);

    timerUIUpdate_ = new wxTimer(this, TIMER_UI_UPDATE);
    timerUIUpdate_->Start(16); // 16 ms (60 fps)
}

MainWindow::~MainWindow()
{
    // It seems that manually deleting widgets is unnecessary.
    // Using delete on wxPanel will cause some delay when the frame closes.
    timerUIUpdate_->Stop();
    delete canvas_;
    delete timerUIUpdate_;
}

inline void MainWindow::CreateOpenGLCanvas()
{
    wxGLAttributes attrs;
    attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
    canvas_ = new OpenGLCanvas(this, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    canvas_->SetFocus();
}

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox1()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Settings");
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

    tcIterCap_ = new wxTextCtrl(box, TC_SET_ITERATION_CAP, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                validIterCap);
    tcInitLearningRate_ = new wxTextCtrl(box, TC_SET_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                         wxTE_CENTER, validLearnRate);
    tcLatWidth_ = new wxTextCtrl(box, TC_SET_LAT_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                 validDimen);
    tcLatHeight_ = new wxTextCtrl(box, TC_SET_LAT_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER,
                                  validDimen);
    *tcIterCap_ << iterationCap_;
    *tcInitLearningRate_ << initLearningRate_;
    *tcLatWidth_ << widthLat_;
    *tcLatHeight_ << heightLat_;

    auto chkBox1 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
    auto chkBox2 = new wxCheckBox(box, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
    chkBox1->SetValue(latFlags_ & LatticeFlags_CyclicX);
    chkBox2->SetValue(latFlags_ & LatticeFlags_CyclicY);

    btnConfirm_ = new wxButton(box, BTN_CONFIRM_AND_RESET, "Confirm and Reset");
    btnConfirm_->Disable();

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);
    auto row4 = new wxBoxSizer(wxHORIZONTAL);
    auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
    row1->Add(iterCapLabel, 1, sizerFlag, 5);
    row1->Add(tcIterCap_, 1, sizerFlag, 5);
    row2->Add(learnRateLabel, 1, sizerFlag, 5);
    row2->Add(tcInitLearningRate_, 1, sizerFlag, 5);
    row3->Add(dimenLabel, 1, sizerFlag, 5);
    row3->Add(tcLatWidth_, 1, sizerFlag, 5);
    row3->Add(tcLatHeight_, 1, sizerFlag, 5);
    row4->Add(chkBox1, 1, sizerFlag, 5);
    row4->Add(chkBox2, 1, sizerFlag, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row4, 0, wxGROW | wxALL, 5);
    boxLayout->Add(btnConfirm_, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox2()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Control");
    auto const box = boxLayout->GetStaticBox();

    auto row1 = new wxBoxSizer(wxHORIZONTAL);
    auto row2 = new wxBoxSizer(wxHORIZONTAL);
    auto row3 = new wxBoxSizer(wxHORIZONTAL);

    btnPlayPause_ = new wxButton(box, BTN_PLAY_PAUSE, "Start");
    btnPlayPause_->Disable();

    btnWatermark_ = new wxButton(box, BTN_WATERMARK, "Watermark");
    btnWatermark_->Disable();

    auto currIterLabel = new wxStaticText(box, wxID_ANY, "Iterations: ");
    tcIterCurr_ = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
    tcIterCurr_->SetCanFocus(false);

    row1->Add(currIterLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row1->Add(tcIterCurr_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
    row2->Add(btnPlayPause_, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    row3->Add(btnWatermark_, 1, wxGROW | wxLEFT | wxRIGHT, 5);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row3, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

inline wxStaticBoxSizer* MainWindow::CreatePanelStaticBox3()
{
    auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "Rendering Options");
    auto const box = boxLayout->GetStaticBox();

    auto row1 = new wxBoxSizer(wxVERTICAL);
    auto row2 = new wxBoxSizer(wxVERTICAL);
    auto chkBox1 = new wxCheckBox(box, CB_RENDEROPT_SURFACE, "Model");
    auto chkBox2 = new wxCheckBox(box, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
    auto chkBox3 = new wxCheckBox(box, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
    auto chkBox4 = new wxCheckBox(box, CB_RENDEROPT_LAT_FACE, "Lattice Face");
    auto chkBox5 = new wxCheckBox(box, CB_RENDEROPT_LIGHT_SOURCE, "Light Source");
    chkBox1->SetValue(canvas_->GetRenderOptionState(RenderOption_Model));
    chkBox2->SetValue(canvas_->GetRenderOptionState(RenderOption_LatticeVertex));
    chkBox3->SetValue(canvas_->GetRenderOptionState(RenderOption_LatticeEdge));
    chkBox4->SetValue(canvas_->GetRenderOptionState(RenderOption_LatticeFace));
    chkBox5->SetValue(canvas_->GetRenderOptionState(RenderOption_LightSource));
    auto surfAlphaLabel = new wxStaticText(box, wxID_ANY, "Model Transparency (%)");
    int const sliderInit = static_cast<int>(100.0f - canvas_->GetModelTransparency() * 100.0f);
    slider_ = new wxSlider(box, SLIDER_TRANSPARENCY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                           wxSL_HORIZONTAL | wxSL_LABELS | wxSL_INVERSE);
    row1->Add(chkBox1, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox2, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox3, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox4, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox5, 0, wxGROW | wxALL, 5);
    row2->Add(surfAlphaLabel, 0, wxGROW);
    row2->Add(slider_, 0, wxGROW);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);

    return boxLayout;
}

void MainWindow::InitializeGL()
{
    canvas_->InitGL();
}

void MainWindow::OnTimerUIUpdate(wxTimerEvent&)
{
    if (tcIterCurr_ != nullptr) {
        assert(world.lattice != nullptr);
        tcIterCurr_->Clear();
        *tcIterCurr_ << world.lattice->CurrentIteration();

        if (world.lattice->IsTrainingDone()) {
            btnWatermark_->Enable();
        } else {
            btnWatermark_->Disable();
        }
    }
    if (canvas_ != nullptr) {
        canvas_->Refresh();
    }
}

void MainWindow::OnButtonPlayPause(wxCommandEvent&)
{
    assert(world.lattice != nullptr);
    world.lattice->ToggleTraining();
    if (btnPlayPause_->GetLabel() == "Start") {
        btnPlayPause_->SetLabel("Pause");
    } else {
        btnPlayPause_->SetLabel("Start");
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
    if (tcLatWidth_->GetValue().ToLong(&tmpLong)) {
        widthLat_ = tmpLong;
    } else {
        SetStatusText("Invalid lattice width!");
        return;
    }
    if (tcLatHeight_->GetValue().ToLong(&tmpLong)) {
        heightLat_ = tmpLong;
    } else {
        SetStatusText("Invalid lattice height!");
        return;
    }
    if (tcIterCap_->GetValue().ToLong(&tmpLong)) {
        iterationCap_ = tmpLong;
    } else {
        SetStatusText("Invalid lattice height!");
        return;
    }
    if (tcInitLearningRate_->GetValue().ToDouble(&tmpDouble)) {
        initLearningRate_ = static_cast<float>(tmpDouble);
    } else {
        SetStatusText("Invalid initial learning rate!");
        return;
    }

    assert(world.dataset != nullptr);
    world.lattice = std::make_unique<Lattice>(widthLat_, heightLat_);
    world.lattice->Train(*world.dataset, initLearningRate_, iterationCap_, latFlags_);
    btnPlayPause_->SetLabel("Start");
    canvas_->ResetLattice();
}

void MainWindow::OnCheckboxInputDataset(wxCommandEvent&)
{
    canvas_->ToggleRenderOption(RenderOption_Model);
}

void MainWindow::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    canvas_->ToggleRenderOption(RenderOption_LatticeVertex);
}

void MainWindow::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    canvas_->ToggleRenderOption(RenderOption_LatticeEdge);
}

void MainWindow::OnCheckboxLatticeFace(wxCommandEvent&)
{
    canvas_->ToggleRenderOption(RenderOption_LatticeFace);
}

void MainWindow::OnCheckboxLightSource(wxCommandEvent&)
{
    canvas_->ToggleRenderOption(RenderOption_LightSource);
}

void MainWindow::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    latFlags_ ^= LatticeFlags_CyclicX;
}

void MainWindow::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    latFlags_ ^= LatticeFlags_CyclicY;
}

void MainWindow::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(slider_->GetMax() - slider_->GetMin());
    float const value = static_cast<float>(slider_->GetValue());
    float const alpha = (range - value) / range;
    canvas_->SetModelColorAlpha(alpha);
}

void MainWindow::OnMenuCameraReset(wxCommandEvent&)
{
    canvas_->ResetCamera();
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
    canvas_->OpenInputDataFile(filepath);

    assert(world.dataset != nullptr);
    world.lattice->Train(*world.dataset, initLearningRate_, iterationCap_, latFlags_);
    btnConfirm_->Enable();
    btnPlayPause_->Enable();
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
