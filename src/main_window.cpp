#include "main_window.hpp"

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(800, 800))
  , canvas_(nullptr)
{
  SetMinSize(wxSize(400, 400));
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

  SetMenuBar(menubar);

  wxPanel* panel = new wxPanel(this, wxID_ANY);
  wxGLAttributes attrs;
  attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
  canvas_ = new OpenGLWindow(this, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

  wxBoxSizer* rootLayout = new wxBoxSizer(wxHORIZONTAL);
  SetSizer(rootLayout);
  Layout();

  rootLayout->Add(panel, 0, wxEXPAND | wxALL, 0);
  rootLayout->Add(canvas_, 1, wxEXPAND | wxALL, 0);

  wxBoxSizer* panelLayout = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(panelLayout);
  wxButton* btn = new wxButton(panel, wxID_ANY, "I'm a wxButton");
  panelLayout->Add(btn, 0, wxEXPAND | wxALL, 10);
};

MainWindow::~MainWindow()
{
  delete canvas_;
}

void
MainWindow::InitializeGL()
{
  canvas_->InitGL();
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
wxEND_EVENT_TABLE()
