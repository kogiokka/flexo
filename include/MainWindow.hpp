#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/timer.h>

#include "Lattice.hpp"
#include "MainPanel.hpp"
#include "OpenGLCanvas.hpp"

class MainWindow : public wxFrame
{
    OpenGLCanvas* m_canvas;
    MainPanel* m_mainPanel;
    wxSizer* m_rootLayout;

    void OnOpenFile(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);

public:
    MainWindow(wxWindow* parent = 0);
    void OnMenuCameraReset(wxCommandEvent& evt);
    void SetMainPanel(MainPanel* panel);
    void SetOpenGLCanvas(OpenGLCanvas* canvas);

    wxDECLARE_EVENT_TABLE();
};

#endif
