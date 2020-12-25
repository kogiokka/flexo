#include "MainWindow.hpp"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/valnum.h>

#include <filesystem>
#include <string>

enum {
  BTN_START = wxID_HIGHEST + 1,
  BTN_PAUSE,
  BTN_CONFIRM_AND_RESET,
  SPCTRL_ITERATION_PER_FRAME,
  CB_RENDEROPT_SURFACE,
  CB_RENDEROPT_LAT_VERTEX,
  CB_RENDEROPT_LAT_EDGE,
  CB_RENDEROPT_LAT_FACE,
  TC_SET_ITERATION_CAP,
  TC_SET_LEARNING_RATE,
  TC_SET_LAT_WIDTH,
  TC_SET_LAT_HEIGHT,
  SLIDER_TRANSPARENCY,
  TIMER_UI_UPDATE,
};

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(1200, 800))
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
  fileMenu->Append(wxID_OPEN, "Open Surface");
  fileMenu->Append(wxID_EXIT, "Exit");

  auto cameraMenu = new wxMenu;
  cameraMenu->Append(wxID_REFRESH, "Reset");

  auto menubar = new wxMenuBar;
  menubar->Append(fileMenu, "&File");
  menubar->Append(cameraMenu, "&Camera");

  this->SetMenuBar(menubar);

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
  delete canvas_;
  delete timerUIUpdate_;
}

inline void
MainWindow::CreateOpenGLCanvas()
{
  wxGLAttributes attrs;
  attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
  canvas_ = new OpenGLCanvas(this, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
  canvas_->SetFocus();
}

inline wxStaticBoxSizer*
MainWindow::CreatePanelStaticBox1()
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

  tcIterCap_ = new wxTextCtrl(
    box, TC_SET_ITERATION_CAP, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validIterCap);
  tcInitLearningRate_ = new wxTextCtrl(
    box, TC_SET_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validLearnRate);
  tcLatWidth_ =
    new wxTextCtrl(box, TC_SET_LAT_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validDimen);
  tcLatHeight_ =
    new wxTextCtrl(box, TC_SET_LAT_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validDimen);
  iterationCap_ = canvas_->GetIterationCap();
  initLearningRate_ = canvas_->GetInitialLearningRate();
  widthLat_ = canvas_->GetLatticeWidth();
  heightLat_ = canvas_->GetLatticeHeight();
  *tcIterCap_ << iterationCap_;
  *tcInitLearningRate_ << initLearningRate_;
  *tcLatWidth_ << widthLat_;
  *tcLatHeight_ << heightLat_;

  auto btnConfirm = new wxButton(box, BTN_CONFIRM_AND_RESET, "Confirm and Reset");

  auto row1 = new wxBoxSizer(wxHORIZONTAL);
  auto row2 = new wxBoxSizer(wxHORIZONTAL);
  auto row3 = new wxBoxSizer(wxHORIZONTAL);
  auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
  row1->Add(iterCapLabel, 1, sizerFlag, 5);
  row1->Add(tcIterCap_, 1, sizerFlag, 5);
  row2->Add(learnRateLabel, 1, sizerFlag, 5);
  row2->Add(tcInitLearningRate_, 1, sizerFlag, 5);
  row3->Add(dimenLabel, 1, sizerFlag, 5);
  row3->Add(tcLatWidth_, 1, sizerFlag, 5);
  row3->Add(tcLatHeight_, 1, sizerFlag, 5);
  boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
  boxLayout->Add(btnConfirm, 0, wxGROW | wxALL, 10);

  return boxLayout;
}

inline wxStaticBoxSizer*
MainWindow::CreatePanelStaticBox2()
{
  auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Control");
  auto const box = boxLayout->GetStaticBox();

  auto row3 = new wxBoxSizer(wxHORIZONTAL);
  auto row2 = new wxBoxSizer(wxHORIZONTAL);
  auto row1 = new wxBoxSizer(wxHORIZONTAL);

  auto btnStart = new wxButton(box, BTN_START, "Start");
  auto btnPause = new wxButton(box, BTN_PAUSE, "Pause");
  auto spctrlIPFLabel = new wxStaticText(box, wxID_ANY, "Iterations Per Frame: ");
  auto spctrlIPF = new wxSpinCtrl(box,
                                  SPCTRL_ITERATION_PER_FRAME,
                                  wxEmptyString,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxALIGN_CENTER_HORIZONTAL,
                                  1,
                                  500,
                                  canvas_->GetIterationsPerFrame());
  auto currIterLabel = new wxStaticText(box, wxID_ANY, "Iterations: ");
  tcIterCurr_ = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
  tcIterCurr_->SetCanFocus(false);

  row1->Add(currIterLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row1->Add(tcIterCurr_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row2->Add(spctrlIPFLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row2->Add(spctrlIPF, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row3->Add(btnStart, 1, wxGROW | wxLEFT | wxRIGHT, 5);
  row3->Add(btnPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
  boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row3, 0, wxGROW | wxALL, 10);

  return boxLayout;
}

inline wxStaticBoxSizer*
MainWindow::CreatePanelStaticBox3()
{
  auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "Rendering Options");
  auto const box = boxLayout->GetStaticBox();

  auto row1 = new wxBoxSizer(wxVERTICAL);
  auto row2 = new wxBoxSizer(wxVERTICAL);
  auto chkBox1 = new wxCheckBox(box, CB_RENDEROPT_SURFACE, "Surface");
  auto chkBox2 = new wxCheckBox(box, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
  auto chkBox3 = new wxCheckBox(box, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
  auto chkBox4 = new wxCheckBox(box, CB_RENDEROPT_LAT_FACE, "Lattice Face");
  chkBox1->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::SURFACE));
  chkBox2->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_VERTEX));
  chkBox3->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_EDGE));
  chkBox4->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_FACE));
  auto surfAlphaLabel = new wxStaticText(box, wxID_ANY, "Surface Transparency (%)");
  int const sliderInit = static_cast<int>(100.0f - canvas_->GetSurfaceTransparency() * 100.0f);
  slider_ = new wxSlider(box,
                         SLIDER_TRANSPARENCY,
                         sliderInit,
                         0,
                         100,
                         wxDefaultPosition,
                         wxDefaultSize,
                         wxSL_HORIZONTAL | wxSL_LABELS | wxSL_INVERSE);
  row1->Add(chkBox1, 0, wxGROW | wxALL, 5);
  row1->Add(chkBox2, 0, wxGROW | wxALL, 5);
  row1->Add(chkBox3, 0, wxGROW | wxALL, 5);
  row1->Add(chkBox4, 0, wxGROW | wxALL, 5);
  row2->Add(surfAlphaLabel, 0, wxGROW);
  row2->Add(slider_, 0, wxGROW);
  boxLayout->Add(row1, 0, wxGROW | wxALL, 10);
  boxLayout->Add(row2, 0, wxGROW | wxALL, 10);

  return boxLayout;
}

void
MainWindow::InitializeGL()
{
  canvas_->InitGL();
}

void
MainWindow::OnTimerUIUpdate(wxTimerEvent&)
{
  if (tcIterCurr_ != nullptr) {
    tcIterCurr_->Clear();
    *tcIterCurr_ << canvas_->GetCurrentIterations();
  }
}

void
MainWindow::OnButtonStart(wxCommandEvent&)
{
  canvas_->SetPlayOrPause(true);
}

void
MainWindow::OnButtonPause(wxCommandEvent&)
{
  canvas_->SetPlayOrPause(false);
}

void
MainWindow::OnSpinCtrlIterationPerFrame(wxSpinEvent& evt)
{
  canvas_->SetIterationsPerFrame(evt.GetInt());
}

void
MainWindow::OnButtonConfirmAndReset(wxCommandEvent&)
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

  canvas_->ResetLattice(widthLat_, heightLat_, iterationCap_, initLearningRate_);
}

void
MainWindow::OnCheckboxSurface(wxCommandEvent&)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::SURFACE);
}

void
MainWindow::OnCheckboxLatticeVertex(wxCommandEvent&)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_VERTEX);
}

void
MainWindow::OnCheckboxLatticeEdge(wxCommandEvent&)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_EDGE);
}

void
MainWindow::OnCheckboxLatticeFace(wxCommandEvent&)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_FACE);
}

void
MainWindow::OnSliderTransparency(wxCommandEvent&)
{
  float const range = static_cast<float>(slider_->GetMax() - slider_->GetMin());
  float const value = static_cast<float>(slider_->GetValue());
  float const alpha = (range - value) / range;
  canvas_->SetSurfaceColorAlpha(alpha);
}

void
MainWindow::OnMenuCameraReset(wxCommandEvent&)
{
  canvas_->ResetCamera();
}

void
MainWindow::OnOpenSurface(wxCommandEvent&)
{
  namespace fs = std::filesystem;
  static wxString defaultDir = "";

  wxFileDialog dialog(
    this, "Import Surface", defaultDir, "", "Wavefront (*.obj)|*.obj", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  dialog.CenterOnParent();

  wxBusyCursor wait;
  if (dialog.ShowModal() == wxID_CANCEL) {
    defaultDir = fs::path(dialog.GetPath().ToStdString()).parent_path().string();
    return;
  }

  // Why wxBusyCursor does not work here?
  // wxBusyCursor wait;
  auto const filepath = dialog.GetPath().ToStdString();
  canvas_->OpenSurface(filepath);
  defaultDir = fs::path(filepath).parent_path().string();
}

void
MainWindow::OnExit([[maybe_unused]] wxCommandEvent& evt)
{
  Close(true);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(wxID_OPEN, MainWindow::OnOpenSurface)
  EVT_MENU(wxID_EXIT, MainWindow::OnExit)
  EVT_MENU(wxID_REFRESH, MainWindow::OnMenuCameraReset)
  EVT_BUTTON(BTN_START, MainWindow::OnButtonStart)
  EVT_BUTTON(BTN_PAUSE, MainWindow::OnButtonPause)
  EVT_BUTTON(BTN_CONFIRM_AND_RESET, MainWindow::OnButtonConfirmAndReset)
  EVT_SPINCTRL(SPCTRL_ITERATION_PER_FRAME, MainWindow::OnSpinCtrlIterationPerFrame)
  EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainWindow::OnCheckboxSurface)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainWindow::OnCheckboxLatticeVertex)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainWindow::OnCheckboxLatticeEdge)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainWindow::OnCheckboxLatticeFace)
  EVT_SLIDER(SLIDER_TRANSPARENCY, MainWindow::OnSliderTransparency)
  EVT_TIMER(TIMER_UI_UPDATE, MainWindow::OnTimerUIUpdate)
wxEND_EVENT_TABLE()
