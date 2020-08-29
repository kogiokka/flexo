#include "MainWindow.hpp"

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(1200, 800))
  , panel_(nullptr)
  , btnStartPause_(nullptr)
  , canvas_(nullptr)
{
  SetMinSize(wxSize(600, 400));
  Center();
  // CreateStatusBar();

  wxMenu* fileMenu = new wxMenu;
  fileMenu->Append(wxID_OPEN, "Open");
  fileMenu->Append(wxID_EXIT, "Exit");

  wxMenu* cameraMenu = new wxMenu;
  cameraMenu->Append(wxID_REFRESH, "Reset");

  wxMenuBar* menubar = new wxMenuBar;
  menubar->Append(fileMenu, "&File");
  menubar->Append(cameraMenu, "&Camera");

  this->SetMenuBar(menubar);

  CreateOpenGLCanvas();

  panel_ = new wxPanel(this, wxID_ANY);

  wxBoxSizer* rootLayout = new wxBoxSizer(wxHORIZONTAL);
  // Proportion: Panel 1 to GLCanvas 3
  rootLayout->Add(panel_, 1, wxGROW | wxALL, 0);
  rootLayout->Add(canvas_, 3, wxGROW | wxALL, 0);
  this->SetSizer(rootLayout);
  this->Layout();

  wxStaticBoxSizer* rowLayout1 = new wxStaticBoxSizer(wxVERTICAL, panel_, "SOM Control");
  btnStartPause_ = new wxButton(rowLayout1->GetStaticBox(), BTN_STARTPAUSE, "Start");
  rowLayout1->Add(btnStartPause_, 1, wxGROW | wxALL, 10);

  wxBoxSizer* panelLayout = new wxBoxSizer(wxVERTICAL);
  panelLayout->Add(rowLayout1, 0, wxGROW | wxALL, 10);
  panel_->SetSizer(panelLayout);
};

MainWindow::~MainWindow()
{
  // It seems that manually deleting widgets is unnecessary.
  // Using delete on wxPanel will cause some delay when the frame closes.
  delete canvas_;
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
MainWindow::OnButtonStartPause(wxCommandEvent& evt)
{
  bool const training = canvas_->GetTrainPause();
  if (training) {
    btnStartPause_->SetLabel("Start");
  } else {
    btnStartPause_->SetLabel("Pause");
  }
  canvas_->ToggleTrainPause(!training);
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
wxEND_EVENT_TABLE()
