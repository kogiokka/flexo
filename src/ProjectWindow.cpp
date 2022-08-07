#include <filesystem>

#include <wx/filedlg.h>
#include <wx/menu.h>

#include "OpenGLCanvas.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"

wxDEFINE_EVENT(EVT_GENERATE_MODEL_DOME, wxCommandEvent);
wxDEFINE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);
wxDEFINE_EVENT(EVT_VIEW_MENU_LATTICE, wxCommandEvent);
wxDEFINE_EVENT(EVT_VIEW_MENU_SOM, wxCommandEvent);
wxDEFINE_EVENT(EVT_VIEW_MENU_WATERMARKING, wxCommandEvent);

// Register factory: ProjectWindow
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto window = new ProjectWindow(nullptr, wxID_ANY, wxDefaultPosition, wxSize(1200, 800), project);
        window->SetMinSize(wxSize(800, 600));
        window->Center();
        window->CreateStatusBar();
        return window;
    }
};

ProjectWindow& ProjectWindow::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<ProjectWindow>(factoryKey);
}

ProjectWindow const& ProjectWindow::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

ProjectWindow::ProjectWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                             WatermarkingProject& project)
    : wxFrame(parent, id, "Self-organizing Map Demo", pos, size)
    , m_project(project)
{
    m_project.SetFrame(this);

    auto fileMenu = new wxMenu;
    fileMenu->Append(wxID_OPEN, "Import model");
    fileMenu->Append(wxID_EXIT, "Exit");

    auto viewMenu = new wxMenu();
    viewMenu->Append(EVT_VIEW_MENU_LATTICE, "Toggle Lattice Panel");
    viewMenu->Append(EVT_VIEW_MENU_SOM, "Toggle SOM Panel");
    viewMenu->Append(EVT_VIEW_MENU_WATERMARKING, "Toggle Watermarking Panel");

    auto cameraMenu = new wxMenu;
    cameraMenu->Append(wxID_REFRESH, "Reset");

    auto modelsMenu = new wxMenu;
    modelsMenu->Append(EVT_GENERATE_MODEL_DOME, "Generate Dome");

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(viewMenu, "&View");
    menubar->Append(cameraMenu, "&Camera");
    menubar->Append(modelsMenu, "&Models");

    this->SetMenuBar(menubar);

    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    m_mainPanel->SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_mainPage = m_mainPanel;

    Bind(wxEVT_MENU, &ProjectWindow::OnOpenFile, this, wxID_OPEN);
    Bind(wxEVT_MENU, &ProjectWindow::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &ProjectWindow::OnMenuCameraReset, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ProjectWindow::OnMenuGenerateModelDome, this, EVT_GENERATE_MODEL_DOME);
    Bind(wxEVT_MENU, &ProjectWindow::OnViewMenu, this, EVT_VIEW_MENU_LATTICE);
    Bind(wxEVT_MENU, &ProjectWindow::OnViewMenu, this, EVT_VIEW_MENU_SOM);
    Bind(wxEVT_MENU, &ProjectWindow::OnViewMenu, this, EVT_VIEW_MENU_WATERMARKING);
}

ProjectWindow::~ProjectWindow()
{
}

WatermarkingProject& ProjectWindow::GetProject()
{
    return m_project;
}

WatermarkingProject const& ProjectWindow::GetProject() const
{
    return m_project;
}

void ProjectWindow::OnOpenFile(wxCommandEvent&)
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

    // Why wxBusyCursor does not work here?
    // wxBusyCursor wait;
    wxString const filepath = dialog.GetPath();
    defaultDir = fs::path(filepath.ToStdString()).parent_path().string();

    wxCommandEvent event(EVT_IMPORT_MODEL);
    event.SetString(filepath);
    ProcessWindowEvent(event);
}

void ProjectWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void ProjectWindow::OnMenuCameraReset(wxCommandEvent&)
{
    OpenGLCanvas::Get(m_project).ResetCamera();
}

// FIXME: Why do I need this?
void ProjectWindow::OnMenuGenerateModelDome(wxCommandEvent&)
{
    wxCommandEvent event(EVT_GENERATE_MODEL_DOME);
    event.SetId(EVT_GENERATE_MODEL_DOME);
    ProcessWindowEvent(event);
}

void ProjectWindow::OnViewMenu(wxCommandEvent& event)
{
    m_project.ProcessEvent(event);
}

wxWindow* ProjectWindow::GetMainPage()
{
    return m_mainPage;
}
