#include "main_window.hpp"

MainWindow::MainWindow(wxWindow* parent)
  : wxFrame(parent, wxID_ANY, "Self-organizing Map: Surface", wxDefaultPosition, wxSize(800, 800))
{
  wxGLAttributes attrs;
  attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
  canvas_ = new OpenGLWindow(this, attrs, wxID_ANY, wxDefaultPosition, GetClientSize(), wxSUNKEN_BORDER);

  Center();

  wxMenu* menu = new wxMenu;
  menu->Append(wxID_EXIT, "Exit");
  wxMenuBar* menubar = new wxMenuBar;
  menubar->Append(menu, "&Menu");

  SetMenuBar(menubar);

  CreateStatusBar();
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
MainWindow::OnExit(wxCommandEvent& event)
{
  Close(true);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
  EVT_MENU(wxID_EXIT, MainWindow::OnExit)
wxEND_EVENT_TABLE()
