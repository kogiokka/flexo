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
#include <wx/valnum.h>

#include <string>

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(1200, 800))
  , panel_(nullptr)
  , tcCurrIter_(nullptr)
  , canvas_(nullptr)
{
  SetMinSize(wxSize(600, 400));
  Center();
  // CreateStatusBar();

  auto fileMenu = new wxMenu;
  fileMenu->Append(wxID_OPEN, "Open surface");
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

  updateTimer_ = new wxTimer(this, TIMER_UI_UPDATE);
  updateTimer_->Start(16); // 16 ms (60 fps)
};

MainWindow::~MainWindow()
{
  // It seems that manually deleting widgets is unnecessary.
  // Using delete on wxPanel will cause some delay when the frame closes.
  delete canvas_;
  delete updateTimer_;
}

inline void
MainWindow::CreateOpenGLCanvas()
{
  wxGLAttributes attrs;
  attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
  canvas_ = new OpenGLCanvas(this, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
}

inline wxStaticBoxSizer* const
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
  validDimen.SetRange(2, 512);
  validLearnRate.SetRange(0.0f, 1.0f);

  tcIterCap_ = new wxTextCtrl(
    box, TC_SET_ITERATION_CAP, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validIterCap);
  tcLearnRate_ = new wxTextCtrl(
    box, TC_SET_LEARNING_RATE, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validLearnRate);
  tcDimen_ =
    new wxTextCtrl(box, TC_SET_DIMENSION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTER, validDimen);
  iterationCap_ = canvas_->GetIterationCap();
  initLearningRate_ = canvas_->GetInitialLearningRate();
  dimension_ = canvas_->GetLatticeDimension();
  *tcIterCap_ << iterationCap_;
  *tcLearnRate_ << initLearningRate_;
  *tcDimen_ << dimension_;

  auto btnConfirm = new wxButton(box, BTN_CONFIRM_AND_RESET, "Confirm and Reset");

  auto row1 = new wxBoxSizer(wxHORIZONTAL);
  auto row2 = new wxBoxSizer(wxHORIZONTAL);
  auto row3 = new wxBoxSizer(wxHORIZONTAL);
  auto sizerFlag = wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT;
  row1->Add(iterCapLabel, 1, sizerFlag, 5);
  row1->Add(tcIterCap_, 1, sizerFlag, 5);
  row2->Add(learnRateLabel, 1, sizerFlag, 5);
  row2->Add(tcLearnRate_, 1, sizerFlag, 5);
  row3->Add(dimenLabel, 1, sizerFlag, 5);
  row3->Add(tcDimen_, 1, sizerFlag, 5);
  boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row3, 0, wxGROW | wxALL, 5);
  boxLayout->Add(btnConfirm, 0, wxGROW | wxALL, 10);

  return boxLayout;
}

inline wxStaticBoxSizer* const
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
  tcCurrIter_ = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
  tcCurrIter_->SetCanFocus(false);

  row1->Add(currIterLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row1->Add(tcCurrIter_, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row2->Add(spctrlIPFLabel, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row2->Add(spctrlIPF, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
  row3->Add(btnStart, 1, wxGROW | wxLEFT | wxRIGHT, 5);
  row3->Add(btnPause, 1, wxGROW | wxLEFT | wxRIGHT, 5);
  boxLayout->Add(row1, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row2, 0, wxGROW | wxALL, 5);
  boxLayout->Add(row3, 0, wxGROW | wxALL, 10);

  return boxLayout;
}

inline wxStaticBoxSizer* const
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
  auto surfAlphaLabel = new wxStaticText(box, wxID_ANY, "Surface Transparency");
  int const sliderInit = canvas_->GetSurfaceTransparency() * 100;
  slider_ = new wxSlider(box, SLIDER_TRANSPARENCY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize);
  slider_->SetToolTip(wxString::Format("%.2f", canvas_->GetSurfaceTransparency()));
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
MainWindow::OnUpdateTimer(wxTimerEvent& evt)
{
  if (tcCurrIter_ != nullptr) {
    tcCurrIter_->Clear();
    *tcCurrIter_ << canvas_->GetCurrentIterations();
  }
}

void
MainWindow::OnButtonStart(wxCommandEvent& evt)
{
  canvas_->SetPlayOrPause(true);
}

void
MainWindow::OnButtonPause(wxCommandEvent& evt)
{
  canvas_->SetPlayOrPause(false);
}

void
MainWindow::OnSpinCtrlIterationPerFrame(wxSpinEvent& evt)
{
  canvas_->SetIterationsPerFrame(evt.GetInt());
}

void
MainWindow::OnButtonConfirmAndReset(wxCommandEvent& evt)
{
  if (tcIterCap_->GetValue().IsEmpty()) {
    *tcIterCap_ << canvas_->GetIterationCap();
  }
  if (tcLearnRate_->GetValue().IsEmpty()) {
    *tcLearnRate_ << canvas_->GetInitialLearningRate();
  }
  if (tcDimen_->GetValue().IsEmpty()) {
    *tcDimen_ << canvas_->GetLatticeDimension();
  }
  canvas_->ResetLattice(iterationCap_, initLearningRate_, dimension_);
}

void
MainWindow::OnEnterIterationCap(wxCommandEvent& evt)
{
  auto const& text = evt.GetString();
  if (text.IsEmpty())
    return;
  iterationCap_ = std::stoi(text.ToStdString());
}

void
MainWindow::OnEnterLearningRate(wxCommandEvent& evt)
{
  auto const& text = evt.GetString();
  if (text.IsEmpty())
    return;
  initLearningRate_ = std::stof(text.ToStdString());
}

void
MainWindow::OnEnterDimension(wxCommandEvent& evt)
{
  auto const& text = evt.GetString();
  if (text.IsEmpty())
    return;
  dimension_ = std::stoi(text.ToStdString());
}

void
MainWindow::OnCheckBoxToggleModel(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::SURFACE);
}

void
MainWindow::OnCheckBoxTogglePoints(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_VERTEX);
}

void
MainWindow::OnCheckBoxToggleLines(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_EDGE);
}

void
MainWindow::OnCheckBoxToggleSurfaces(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LAT_FACE);
}

void
MainWindow::OnSliderTransparency(wxCommandEvent& evt)
{
  float const range = static_cast<float>(slider_->GetMax() - slider_->GetMin());
  float const value = static_cast<float>(slider_->GetValue());
  float const alpha = value / range;
  canvas_->SetSurfaceTransparency(alpha);
  slider_->SetToolTip(wxString::Format("%.2f", alpha));
}

void
MainWindow::ResetCamera(wxCommandEvent& evt)
{
  canvas_->ResetCamera();
}

void
MainWindow::OnOpen(wxCommandEvent& evt)
{
  wxFileDialog dialog(this, "Import Wavefront .obj file", wxEmptyString, wxEmptyString, "*.obj");
  dialog.SetDirectory(wxGetCwd());
  dialog.CenterOnParent();
  if (dialog.ShowModal() == wxID_OK) {
    canvas_->OpenSurface(dialog.GetPath().ToStdString());
  }
}

void
MainWindow::OnExit(wxCommandEvent& evt)
{
  Close(true);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(wxID_OPEN, MainWindow::OnOpen)
  EVT_MENU(wxID_EXIT, MainWindow::OnExit)
  EVT_MENU(wxID_REFRESH, MainWindow::ResetCamera)
  EVT_TEXT(TC_SET_ITERATION_CAP, MainWindow::OnEnterIterationCap)
  EVT_TEXT(TC_SET_LEARNING_RATE, MainWindow::OnEnterLearningRate)
  EVT_TEXT(TC_SET_DIMENSION, MainWindow::OnEnterDimension)
  EVT_BUTTON(BTN_START, MainWindow::OnButtonStart)
  EVT_BUTTON(BTN_PAUSE, MainWindow::OnButtonPause)
  EVT_BUTTON(BTN_CONFIRM_AND_RESET, MainWindow::OnButtonConfirmAndReset)
  EVT_SPINCTRL(SPCTRL_ITERATION_PER_FRAME, MainWindow::OnSpinCtrlIterationPerFrame)
  EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainWindow::OnCheckBoxToggleModel)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainWindow::OnCheckBoxTogglePoints)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainWindow::OnCheckBoxToggleLines)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainWindow::OnCheckBoxToggleSurfaces)
  EVT_SLIDER(SLIDER_TRANSPARENCY, MainWindow::OnSliderTransparency)
  EVT_TIMER(TIMER_UI_UPDATE, MainWindow::OnUpdateTimer)
wxEND_EVENT_TABLE()
