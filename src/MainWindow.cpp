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
  wxStaticBoxSizer* rowLayout2 = new wxStaticBoxSizer(wxVERTICAL, panel_, "Rendering Options");
  btnStartPause_ = new wxButton(rowLayout1->GetStaticBox(), BTN_STARTPAUSE, "Start");
  rowLayout1->Add(btnStartPause_, 1, wxGROW | wxALL, 10);
  wxCheckBox* chkBox1 = new wxCheckBox(rowLayout2->GetStaticBox(), CB_TOGGLE_MODEL, "Toggle Model");
  wxCheckBox* chkBox2 = new wxCheckBox(rowLayout2->GetStaticBox(), CB_TOGGLE_POINTS, "Toggle Grid Points");
  wxCheckBox* chkBox3 = new wxCheckBox(rowLayout2->GetStaticBox(), CB_TOGGLE_LINES, "Toggle Grid Lines");
  wxCheckBox* chkBox4 = new wxCheckBox(rowLayout2->GetStaticBox(), CB_TOGGLE_SURFACES, "Toggle Grid Surface");
  chkBox1->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::MODEL));
  chkBox2->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::POINTS));
  chkBox3->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::LINES));
  chkBox4->SetValue(canvas_->GetRenderOptionState(OpenGLCanvas::RenderOpt::SURFACE));
  rowLayout2->Add(chkBox1, 1, wxGROW | wxALL, 10);
  rowLayout2->Add(chkBox2, 1, wxGROW | wxALL, 10);
  rowLayout2->Add(chkBox3, 1, wxGROW | wxALL, 10);
  rowLayout2->Add(chkBox4, 1, wxGROW | wxALL, 10);

  wxBoxSizer* panelLayout = new wxBoxSizer(wxVERTICAL);
  panelLayout->Add(rowLayout1, 0, wxGROW | wxALL, 10);
  panelLayout->Add(rowLayout2, 0, wxGROW | wxALL, 10);
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
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::MODEL);
}

void
MainWindow::OnCheckBoxTogglePoints(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::POINTS);
}

void
MainWindow::OnCheckBoxToggleLines(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::LINES);
}

void
MainWindow::OnCheckBoxToggleSurfaces(wxCommandEvent& evt)
{
  canvas_->ToggleRenderOption(OpenGLCanvas::RenderOpt::SURFACE);
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
  EVT_CHECKBOX(CB_TOGGLE_MODEL, MainWindow::OnCheckBoxToggleModel)
  EVT_CHECKBOX(CB_TOGGLE_POINTS, MainWindow::OnCheckBoxTogglePoints)
  EVT_CHECKBOX(CB_TOGGLE_LINES, MainWindow::OnCheckBoxToggleLines)
  EVT_CHECKBOX(CB_TOGGLE_SURFACES, MainWindow::OnCheckBoxToggleSurfaces)
wxEND_EVENT_TABLE()
