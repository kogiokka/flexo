#include "MainWindow.hpp"

#include <wx/checkbox.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valnum.h>

#include <string>

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(1200, 800))
  , panel_(nullptr)
  , btnStartPause_(nullptr)
  , tcCurrIter_(nullptr)
  , canvas_(nullptr)
{
  SetMinSize(wxSize(600, 400));
  Center();
  // CreateStatusBar();

  auto fileMenu = new wxMenu;
  fileMenu->Append(wxID_OPEN, "Open");
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
  boxLayout->Add(row1, 1, wxGROW | wxALL, 5);
  boxLayout->Add(row2, 1, wxGROW | wxALL, 5);
  boxLayout->Add(row3, 1, wxGROW | wxALL, 5);
  boxLayout->Add(btnConfirm, 1, wxGROW | wxALL, 5);

  return boxLayout;
}

inline wxStaticBoxSizer* const
MainWindow::CreatePanelStaticBox2()
{
  auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Control");
  auto const box = boxLayout->GetStaticBox();

  btnStartPause_ = new wxButton(box, BTN_STARTPAUSE, "Start");
  auto currIterSizer = new wxBoxSizer(wxHORIZONTAL);
  auto stCurrIter = new wxStaticText(box, wxID_ANY, "Iterations: ");
  tcCurrIter_ = new wxTextCtrl(box, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
  tcCurrIter_->SetCanFocus(false);
  currIterSizer->Add(stCurrIter, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  currIterSizer->Add(tcCurrIter_, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  boxLayout->Add(currIterSizer, 1, wxGROW | wxALL, 10);
  boxLayout->Add(btnStartPause_, 1, wxGROW | wxALL, 10);

  return boxLayout;
}

inline wxStaticBoxSizer* const
MainWindow::CreatePanelStaticBox3()
{
  auto const boxLayout = new wxStaticBoxSizer(wxVERTICAL, panel_, "Rendering Options");
  auto const box = boxLayout->GetStaticBox();

  auto chkBox1 = new wxCheckBox(box, CB_RENDEROPT_SURFACE, "Surface");
  auto chkBox2 = new wxCheckBox(box, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
  auto chkBox3 = new wxCheckBox(box, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
  auto chkBox4 = new wxCheckBox(box, CB_RENDEROPT_LAT_FACE, "Lattice Face");
  chkBox1->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::SURFACE));
  chkBox2->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_VERTEX));
  chkBox3->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_EDGE));
  chkBox4->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_FACE));
  boxLayout->Add(chkBox1, 1, wxGROW | wxALL, 5);
  boxLayout->Add(chkBox2, 1, wxGROW | wxALL, 5);
  boxLayout->Add(chkBox3, 1, wxGROW | wxALL, 5);
  boxLayout->Add(chkBox4, 1, wxGROW | wxALL, 5);

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
MainWindow::OnButtonStartPause(wxCommandEvent& evt)
{
  bool const training = canvas_->GetPlayPause();
  if (training) {
    btnStartPause_->SetLabel("Start");
  } else {
    btnStartPause_->SetLabel("Pause");
  }
  canvas_->TogglePlayPause(!training);
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
  btnStartPause_->SetLabel("Start");
}

void
MainWindow::OnEnterIterationCap(wxCommandEvent& evt)
{
  auto const& text = tcIterCap_->GetValue();
  if (text.IsEmpty())
    return;
  iterationCap_ = std::stoi(text.ToStdString());
}

void
MainWindow::OnEnterLearningRate(wxCommandEvent& evt)
{
  auto const& text = tcLearnRate_->GetValue();
  if (text.IsEmpty())
    return;
  initLearningRate_ = std::stof(text.ToStdString());
}

void
MainWindow::OnEnterDimension(wxCommandEvent& evt)
{
  auto const& text = tcDimen_->GetValue();
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
MainWindow::ResetCamera(wxCommandEvent& evt)
{
  canvas_->ResetCamera();
}

void
MainWindow::OnExit(wxCommandEvent& evt)
{
  Close(true);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(wxID_EXIT, MainWindow::OnExit)
  EVT_MENU(wxID_REFRESH, MainWindow::ResetCamera)
  EVT_TEXT(TC_SET_ITERATION_CAP, MainWindow::OnEnterIterationCap)
  EVT_TEXT(TC_SET_LEARNING_RATE, MainWindow::OnEnterLearningRate)
  EVT_TEXT(TC_SET_DIMENSION, MainWindow::OnEnterDimension)
  EVT_BUTTON(BTN_STARTPAUSE, MainWindow::OnButtonStartPause)
  EVT_BUTTON(BTN_CONFIRM_AND_RESET, MainWindow::OnButtonConfirmAndReset)
  EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainWindow::OnCheckBoxToggleModel)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainWindow::OnCheckBoxTogglePoints)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainWindow::OnCheckBoxToggleLines)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainWindow::OnCheckBoxToggleSurfaces)
  EVT_TIMER(TIMER_UI_UPDATE, MainWindow::OnUpdateTimer)
wxEND_EVENT_TABLE()
