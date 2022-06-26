#include <filesystem>
#include <string>
#include <vector>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/valnum.h>

#include "MainWindow.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

MainWindow::MainWindow(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Self-organizing Map Demo", wxDefaultPosition, wxSize(1200, 800))
    , m_canvas(nullptr)
{
    SetMinSize(wxSize(600, 400));
    Center();
    CreateStatusBar();

    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "Open Model");
    fileMenu->Append(wxID_EXIT, "Exit");

    auto cameraMenu = new wxMenu;
    cameraMenu->Append(wxID_REFRESH, "Reset");

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(cameraMenu, "&Camera");

    this->SetMenuBar(menubar);

    m_rootLayout = new wxBoxSizer(wxHORIZONTAL);
    SetSizer(m_rootLayout);
}

void MainWindow::OnMenuCameraReset(wxCommandEvent&)
{
    m_canvas->ResetCamera();
}

void MainWindow::OnOpenFile(wxCommandEvent&)
{
    namespace fs = std::filesystem;
    static wxString defaultDir = "";

    wxFileDialog dialog(this, "Import Input Dataset", defaultDir, "",
                        "Volumetric models|*.toml|Polygonal models (*stl,*.obj)|*stl;*.obj",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    dialog.CenterOnParent();

    wxBusyCursor wait;
    if (dialog.ShowModal() == wxID_CANCEL) {
        defaultDir = fs::path(dialog.GetPath().ToStdString()).parent_path().string();
        return;
    }

    assert(m_canvas != nullptr);

    // Why wxBusyCursor does not work here?
    // wxBusyCursor wait;
    auto const filepath = dialog.GetPath().ToStdString();
    defaultDir = fs::path(filepath).parent_path().string();
    m_canvas->OpenInputDataFile(filepath);

    assert(m_mainPanel != nullptr);
    assert(world.dataset != nullptr);

    m_mainPanel->SetupTraning();
    m_mainPanel->m_btnConfirm->Enable();
    m_mainPanel->m_btnPlayPause->Enable();
}

void MainWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainWindow::SetMainPanel(MainPanel* panel)
{
    m_mainPanel = panel;
    m_rootLayout->Add(panel, 1, wxGROW | wxALL, 0);
    Layout();
}

void MainWindow::SetOpenGLCanvas(OpenGLCanvas* canvas)
{
    m_canvas = canvas;
    m_rootLayout->Add(canvas, 3, wxGROW | wxALL, 0);
    Layout();
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(wxID_OPEN, MainWindow::OnOpenFile)
    EVT_MENU(wxID_EXIT, MainWindow::OnExit)
    EVT_MENU(wxID_REFRESH, MainWindow::OnMenuCameraReset)
wxEND_EVENT_TABLE()
