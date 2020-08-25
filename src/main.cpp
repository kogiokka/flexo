#include "main_window.hpp"

#include <wx/app.h>

class SurfaceSOM : public wxApp
{
public:
  virtual bool OnInit() override;
  virtual int OnExit() override;
};

bool
SurfaceSOM::OnInit()
{
  if (!wxApp::OnInit()) {
    return false;
  }

  MainWindow* window = new MainWindow();
  window->Show();
  window->InitializeGL();

  return true;
}

int
SurfaceSOM::OnExit()
{
  return wxApp::OnExit();
}

IMPLEMENT_APP(SurfaceSOM);
