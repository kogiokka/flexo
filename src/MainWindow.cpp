#include <filesystem>
#include <string>
#include <vector>

#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/utils.h>

#include "MainWindow.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

wxDEFINE_EVENT(CMD_GENERATE_MODEL_DOME, wxCommandEvent);
wxDEFINE_EVENT(CMD_IMPORT_MODEL, wxCommandEvent);

MainWindow::MainWindow(wxWindow* parent)
    : wxFrame(parent, wxID_ANY, "Self-organizing Map Demo", wxDefaultPosition, wxSize(1200, 800))
    , m_canvas(nullptr)
{
    SetMinSize(wxSize(600, 400));
    Center();
    CreateStatusBar();

    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "Import model");
    fileMenu->Append(wxID_EXIT, "Exit");

    auto cameraMenu = new wxMenu;
    cameraMenu->Append(wxID_REFRESH, "Reset");

    auto modelsMenu = new wxMenu;
    modelsMenu->Append(CMD_GENERATE_MODEL_DOME, "Generate Dome");

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(cameraMenu, "&Camera");
    menubar->Append(modelsMenu, "&Models");

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
    wxString const filepath = dialog.GetPath();
    defaultDir = fs::path(filepath.ToStdString()).parent_path().string();

    wxCommandEvent event(CMD_IMPORT_MODEL);
    event.SetString(filepath);
    ProcessWindowEvent(event);
}

void MainWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainWindow::SetMainPanel(MainPanel* panel)
{
    m_rootLayout->Add(panel, 1, wxGROW | wxALL, 0);
    Layout();
}

void MainWindow::SetOpenGLCanvas(OpenGLCanvas* canvas)
{
    m_canvas = canvas;
    m_rootLayout->Add(canvas, 3, wxGROW | wxALL, 0);
    Layout();
}

// FIXME: Why do I need this?
void MainWindow::OnMenuGenerateModelDome(wxCommandEvent&)
{
    wxCommandEvent event(CMD_GENERATE_MODEL_DOME);
    event.SetId(CMD_GENERATE_MODEL_DOME);
    ProcessWindowEvent(event);
}

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(wxID_OPEN, MainWindow::OnOpenFile)
    EVT_MENU(wxID_EXIT, MainWindow::OnExit)
    EVT_MENU(wxID_REFRESH, MainWindow::OnMenuCameraReset)
    EVT_MENU(CMD_GENERATE_MODEL_DOME, MainWindow::OnMenuGenerateModelDome)
wxEND_EVENT_TABLE()
