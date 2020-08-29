#include "MainWindow.hpp"

#include <wx/checkbox.h>
#include <wx/glcanvas.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(1200, 800))
  , panel_(nullptr)
  , btnStartPause_(nullptr)
  , currIter_(nullptr)
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

  auto const rowLayout1 = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Control");
  auto const rowLayout2 = new wxStaticBoxSizer(wxVERTICAL, panel_, "Rendering Options");
  auto const statBox1 = rowLayout1->GetStaticBox();
  auto const statBox2 = rowLayout2->GetStaticBox();

  btnStartPause_ = new wxButton(rowLayout1->GetStaticBox(), BTN_STARTPAUSE, "Start");
  auto currIterSizer = new wxBoxSizer(wxHORIZONTAL);
  auto stCurrIter = new wxStaticText(statBox1, wxID_ANY, "Iterations: ");
  currIter_ = new wxTextCtrl(statBox1, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_CENTER);
  currIter_->SetCanFocus(false);
  currIterSizer->Add(stCurrIter, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  currIterSizer->Add(currIter_, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  rowLayout1->Add(currIterSizer, 1, wxGROW | wxALL, 10);
  rowLayout1->Add(btnStartPause_, 1, wxGROW | wxALL, 10);
  auto chkBox1 = new wxCheckBox(statBox2, CB_RENDEROPT_SURFACE, "Surface");
  auto chkBox2 = new wxCheckBox(statBox2, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
  auto chkBox3 = new wxCheckBox(statBox2, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
  auto chkBox4 = new wxCheckBox(statBox2, CB_RENDEROPT_LAT_FACE, "Lattice Face");
  chkBox1->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::SURFACE));
  chkBox2->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_VERTEX));
  chkBox3->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_EDGE));
  chkBox4->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LAT_FACE));
  rowLayout2->Add(chkBox1, 1, wxGROW | wxALL, 5);
  rowLayout2->Add(chkBox2, 1, wxGROW | wxALL, 5);
  rowLayout2->Add(chkBox3, 1, wxGROW | wxALL, 5);
  rowLayout2->Add(chkBox4, 1, wxGROW | wxALL, 5);

  auto panelLayout = new wxBoxSizer(wxVERTICAL);
  panelLayout->Add(rowLayout1, 0, wxGROW | wxALL, 10);
  panelLayout->Add(rowLayout2, 0, wxGROW | wxALL, 10);
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

void
MainWindow::InitializeGL()
{
  canvas_->InitGL();
}

void
MainWindow::OnUpdateTimer(wxTimerEvent& evt)
{
  if (currIter_ != nullptr) {
    currIter_->Clear();
    *currIter_ << canvas_->GetCurrentIterations();
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
  EVT_BUTTON(BTN_STARTPAUSE, MainWindow::OnButtonStartPause)
  EVT_CHECKBOX(CB_RENDEROPT_SURFACE, MainWindow::OnCheckBoxToggleModel)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, MainWindow::OnCheckBoxTogglePoints)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, MainWindow::OnCheckBoxToggleLines)
  EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, MainWindow::OnCheckBoxToggleSurfaces)
  EVT_TIMER(TIMER_UI_UPDATE, MainWindow::OnUpdateTimer)
wxEND_EVENT_TABLE()
