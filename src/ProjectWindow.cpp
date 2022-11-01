#include <filesystem>

#include <wx/artprov.h>
#include <wx/aui/framemanager.h>
#include <wx/filedlg.h>
#include <wx/menu.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "common/Logger.hpp"
#include "pane/SceneViewportPane.hpp"

wxDEFINE_EVENT(EVT_OPEN_MODEL, wxCommandEvent);
wxDEFINE_EVENT(EVT_OPEN_IMAGE, wxCommandEvent);
wxDEFINE_EVENT(EVT_ADD_UV_SPHERE, wxCommandEvent);
wxDEFINE_EVENT(EVT_ADD_PLATE_50_BY_50, wxCommandEvent);
wxDEFINE_EVENT(EVT_ADD_PLATE_100_BY_100, wxCommandEvent);
wxDEFINE_EVENT(EVT_ADD_PLATE_200_BY_200, wxCommandEvent);
wxDEFINE_EVENT(EVT_SCREENSHOT, wxCommandEvent);
wxDEFINE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

enum {
    TIMER_UPDATE_UI = wxID_HIGHEST + 1,
    EVT_VIEW_MENU_SCENE_VIEWPORT,
    EVT_VIEW_MENU_SOM,
    EVT_VIEW_MENU_WATERMARKING,
    EVT_VIEW_MENU_SCENE_OUTLINER,
};

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

    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    m_mainPanel->SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_mainPage = m_mainPanel;
    m_mgr.SetManagedWindow(m_mainPage);

    auto fileMenu = new wxMenu;
    auto* openModelItem = new wxMenuItem(fileMenu, EVT_OPEN_MODEL, "Open model", "");
    auto* openImageItem = new wxMenuItem(fileMenu, EVT_OPEN_IMAGE, "Open image", "");
    openModelItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    openImageItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    fileMenu->Append(openModelItem);
    fileMenu->Append(openImageItem);
    fileMenu->Append(wxID_EXIT, "Exit");

    m_viewMenu = new wxMenu();
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SCENE_VIEWPORT, "Toggle 3D Viewport");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SCENE_OUTLINER, "Toggle Scene Outliner");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SOM, "Toggle SOM Pane");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_WATERMARKING, "Toggle Watermarking Pane");

    auto cameraMenu = new wxMenu;
    cameraMenu->Append(wxID_REFRESH, "Reset");

    auto modelsMenu = new wxMenu;
    modelsMenu->Append(EVT_ADD_UV_SPHERE, "Generate UV Sphere");

    auto plateMenu = new wxMenu;
    plateMenu->Append(EVT_ADD_PLATE_50_BY_50, "Add 50x50 Plate");
    plateMenu->Append(EVT_ADD_PLATE_100_BY_100, "Add 100x100 Plate");
    plateMenu->Append(EVT_ADD_PLATE_200_BY_200, "Add 200x200 Plate");

    auto* screenshotMenu = new wxMenu;
    auto* itemScreenshotViewport = new wxMenuItem(screenshotMenu, EVT_SCREENSHOT, "Screenshot 3D Viewport", "");
    itemScreenshotViewport->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE));
    screenshotMenu->Append(itemScreenshotViewport);

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(m_viewMenu, "&View");
    menubar->Append(cameraMenu, "&Camera");
    menubar->Append(modelsMenu, "&Add");
    menubar->Append(plateMenu, "&Plate");
    menubar->Append(screenshotMenu, "&Screeenshot");

    this->SetMenuBar(menubar);

    Bind(wxEVT_MENU, &ProjectWindow::OnOpenModelFile, this, EVT_OPEN_MODEL);
    Bind(wxEVT_MENU, &ProjectWindow::OnOpenImageFile, this, EVT_OPEN_IMAGE);
    Bind(wxEVT_MENU, &ProjectWindow::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &ProjectWindow::OnMenuCameraReset, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ProjectWindow::OnMenuGenerateModelDome, this, EVT_ADD_UV_SPHERE);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_ADD_PLATE_50_BY_50);
            event.SetInt(50);
            m_project.ProcessEvent(event);
        },
        EVT_ADD_PLATE_50_BY_50);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_ADD_PLATE_100_BY_100);
            event.SetInt(100);
            m_project.ProcessEvent(event);
        },
        EVT_ADD_PLATE_100_BY_100);
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_ADD_PLATE_200_BY_200);
            event.SetInt(200);
            m_project.ProcessEvent(event);
        },
        EVT_ADD_PLATE_200_BY_200);

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_SCREENSHOT);
            m_project.ProcessEvent(event);
        },
        EVT_SCREENSHOT);

    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SCENE_VIEWPORT);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SOM);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_WATERMARKING);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SCENE_OUTLINER);

    m_updateUITimer = new wxTimer(this, TIMER_UPDATE_UI);
    m_updateUITimer->Start(16);
    Bind(wxEVT_TIMER, &ProjectWindow::OnTimerUpdateUI, this);
}

ProjectWindow::~ProjectWindow()
{
    m_updateUITimer->Stop();
}

wxAuiManager& ProjectWindow::GetPaneManager()
{
    return m_mgr;
}

WatermarkingProject& ProjectWindow::GetProject()
{
    return m_project;
}

WatermarkingProject const& ProjectWindow::GetProject() const
{
    return m_project;
}

void ProjectWindow::OnOpenModelFile(wxCommandEvent&)
{
    namespace fs = std::filesystem;
    static wxString defaultDir = "";

    wxFileDialog dialog(this, "Import Input Dataset", defaultDir, "", "Volumetric model (.rvl)|*.rvl",
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

    m_project.ImportVolumetricModel(filepath);
}

void ProjectWindow::OnOpenImageFile(wxCommandEvent&)
{
    namespace fs = std::filesystem;
    static wxString defaultDir = "";
    wxFileDialog dialog(this, "Open Image", defaultDir, "", "PNG Image|*.png", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    dialog.CenterOnParent();

    wxBusyCursor wait;
    if (dialog.ShowModal() == wxID_CANCEL) {
        defaultDir = fs::path(dialog.GetPath().ToStdString()).parent_path().string();
        return;
    }
    wxString const filepath = dialog.GetPath();
    defaultDir = fs::path(filepath.ToStdString()).parent_path().string();

    wxCommandEvent event(EVT_OPEN_IMAGE);
    event.SetString(filepath);
    m_project.ProcessEvent(event);
}

void ProjectWindow::OnExit(wxCommandEvent&)
{
    Close(true);
}

void ProjectWindow::OnMenuCameraReset(wxCommandEvent&)
{
    SceneViewportPane::Get(m_project).ResetCamera();
}

// FIXME: Why do I need this?
void ProjectWindow::OnMenuGenerateModelDome(wxCommandEvent&)
{
    wxCommandEvent event(EVT_ADD_UV_SPHERE);
    event.SetId(EVT_ADD_UV_SPHERE);
    m_project.ProcessEvent(event);
}

wxWindow* ProjectWindow::GetMainPage()
{
    return m_mainPage;
}

void ProjectWindow::OnTimerUpdateUI(wxTimerEvent&)
{
    m_viewMenu->Check(EVT_VIEW_MENU_SCENE_VIEWPORT, m_mgr.GetPane("viewport").IsShown());
    m_viewMenu->Check(EVT_VIEW_MENU_SCENE_OUTLINER, m_mgr.GetPane("outliner").IsShown());
    m_viewMenu->Check(EVT_VIEW_MENU_SOM, m_mgr.GetPane("som").IsShown());
    m_viewMenu->Check(EVT_VIEW_MENU_WATERMARKING, m_mgr.GetPane("watermarking").IsShown());

    UpdateWindowUI();
}

void ProjectWindow::OnTogglePane(wxCommandEvent& event)
{
    wxString name;
    int const id = event.GetId();

    if (id == EVT_VIEW_MENU_SOM) {
        name = "som";
    } else if (id == EVT_VIEW_MENU_WATERMARKING) {
        name = "watermarking";
    } else if (id == EVT_VIEW_MENU_SCENE_OUTLINER) {
        name = "outliner";
    } else if (id == EVT_VIEW_MENU_SCENE_VIEWPORT) {
        name = "viewport";
    }

    wxAuiPaneInfo& paneInfo = m_mgr.GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_mgr.Update();
}
