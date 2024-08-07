#include <filesystem>

#include <wx/artprov.h>
#include <wx/aui/framemanager.h>
#include <wx/filedlg.h>
#include <wx/menu.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SceneController.hpp"
#include "dialog/ViewportSettingsDialog.hpp"
#include "log/Logger.h"
#include "pane/SceneViewportPane.hpp"

wxDEFINE_EVENT(EVT_OPEN_MODEL, wxCommandEvent);
wxDEFINE_EVENT(EVT_SCREENSHOT, wxCommandEvent);
wxDEFINE_EVENT(EVT_IMPORT_MODEL, wxCommandEvent);

wxDEFINE_EVENT(EVT_MENU_CAMERA_PERSPECTIVE, wxCommandEvent);
wxDEFINE_EVENT(EVT_MENU_CAMERA_ORTHOGONAL, wxCommandEvent);
wxDEFINE_EVENT(EVT_MENU_BACKGROUND_DARK, wxCommandEvent);
wxDEFINE_EVENT(EVT_MENU_BACKGROUND_LIGHT, wxCommandEvent);

enum {
    TIMER_UPDATE_UI = wxID_HIGHEST + 1,
    EVT_VIEW_MENU_SCENE_VIEWPORT,
    EVT_VIEW_MENU_SOM,
    EVT_VIEW_MENU_PROPERTIES,
    EVT_VIEW_MENU_SCENE_OUTLINER,
    EVT_VIEW_MENU_VIEWPORT_SETTINGS,
};

// Register factory: ProjectWindow
static FlexoProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> wxWeakRef<wxWindow> {
        auto window = new ProjectWindow(nullptr, wxID_ANY, wxDefaultPosition, wxSize(1200, 800), project);
        window->SetMinSize(wxSize(800, 600));
        window->Center();
        window->CreateStatusBar();
        return window;
    }
};

ProjectWindow& ProjectWindow::Get(FlexoProject& project)
{
    return project.AttachedWindows::Get<ProjectWindow>(factoryKey);
}

ProjectWindow const& ProjectWindow::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

ProjectWindow::ProjectWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                             FlexoProject& project)
    : wxFrame(parent, id, "Flexo", pos, size)
    , m_project(project)
{
    m_project.SetWindow(this);

    m_mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    m_mainPanel->SetSizer(new wxBoxSizer(wxHORIZONTAL));

    m_mainPage = m_mainPanel;
    m_mgr.SetManagedWindow(m_mainPage);

    auto fileMenu = new wxMenu;
    auto* openModelItem = new wxMenuItem(fileMenu, EVT_OPEN_MODEL, "Open model", "");
    openModelItem->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    fileMenu->Append(openModelItem);
    fileMenu->Append(wxID_EXIT, "Exit");

    m_viewMenu = new wxMenu();
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SCENE_VIEWPORT, "Toggle 3D Viewport");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SCENE_OUTLINER, "Toggle Scene Outliner");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_SOM, "Toggle SOM Pane");
    m_viewMenu->AppendCheckItem(EVT_VIEW_MENU_PROPERTIES, "Toggle Properties Pane");
    m_viewMenu->AppendSeparator();
    m_viewMenu->Append(EVT_VIEW_MENU_VIEWPORT_SETTINGS, "Viewport Settings");

    auto cameraMenu = new wxMenu;
    auto perspItem = cameraMenu->AppendRadioItem(EVT_MENU_CAMERA_PERSPECTIVE, "Perspective");
    perspItem->Check();
    cameraMenu->AppendRadioItem(EVT_MENU_CAMERA_ORTHOGONAL, "Orthogonal");
    cameraMenu->Append(wxID_REFRESH, "Reset");
    cameraMenu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_MENU_CAMERA_PERSPECTIVE);
            m_project.ProcessEvent(event);
        },
        EVT_MENU_CAMERA_PERSPECTIVE);
    cameraMenu->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_MENU_CAMERA_ORTHOGONAL);
            m_project.ProcessEvent(event);
        },
        EVT_MENU_CAMERA_ORTHOGONAL);
    cameraMenu->Bind(
        wxEVT_MENU,
        [this, perspItem](wxCommandEvent&) {
            SceneViewportPane::Get(m_project).ResetCamera();
            perspItem->Check();
        },
        wxID_REFRESH);

    auto addMenu = new wxMenu;
#define X(evt, name) addMenu->Append(evt, name);
    ADD_OBJECT_LIST
#undef X

    auto* screenshotMenu = new wxMenu;
    auto* itemScreenshotViewport = new wxMenuItem(screenshotMenu, EVT_SCREENSHOT, "Screenshot 3D Viewport", "");
    itemScreenshotViewport->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE));
    screenshotMenu->Append(itemScreenshotViewport);

    auto menubar = new wxMenuBar;
    menubar->Append(fileMenu, "&File");
    menubar->Append(m_viewMenu, "&View");
    menubar->Append(cameraMenu, "&Camera");
    menubar->Append(addMenu, "&Add");
    // menubar->Append(screenshotMenu, "&Screeenshot");

    this->SetMenuBar(menubar);

    Bind(wxEVT_MENU, &ProjectWindow::OnOpenModelFile, this, EVT_OPEN_MODEL);
    Bind(wxEVT_MENU, &ProjectWindow::OnExit, this, wxID_EXIT);

#define X(evt, name)                                                                                                   \
    Bind(                                                                                                              \
        wxEVT_MENU,                                                                                                    \
        [this](wxCommandEvent&) {                                                                                      \
            wxCommandEvent event(evt);                                                                                 \
            event.SetId(evt);                                                                                          \
            m_project.ProcessEvent(event);                                                                             \
        },                                                                                                             \
        evt);
    ADD_OBJECT_LIST
#undef X

    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&) {
            wxCommandEvent event(EVT_SCREENSHOT);
            m_project.ProcessEvent(event);
        },
        EVT_SCREENSHOT);

    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SCENE_VIEWPORT);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SOM);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_PROPERTIES);
    Bind(wxEVT_MENU, &ProjectWindow::OnTogglePane, this, EVT_VIEW_MENU_SCENE_OUTLINER);

    Bind(wxEVT_MENU, &ProjectWindow::OnViewportSettings, this, EVT_VIEW_MENU_VIEWPORT_SETTINGS);

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

FlexoProject& ProjectWindow::GetProject()
{
    return m_project;
}

FlexoProject const& ProjectWindow::GetProject() const
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

    wxCommandEvent event(EVT_IMPORT_MODEL);
    event.SetString(filepath);
    m_project.ProcessEvent(event);

}

void ProjectWindow::OnExit(wxCommandEvent&)
{
    Close(true);
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
    m_viewMenu->Check(EVT_VIEW_MENU_PROPERTIES, m_mgr.GetPane("properties").IsShown());

    UpdateWindowUI();
}

void ProjectWindow::OnTogglePane(wxCommandEvent& event)
{
    wxString name;
    int const id = event.GetId();

    // FIXME
    if (id == EVT_VIEW_MENU_SOM) {
        name = "som";
    } else if (id == EVT_VIEW_MENU_SCENE_OUTLINER) {
        name = "outliner";
    } else if (id == EVT_VIEW_MENU_SCENE_VIEWPORT) {
        name = "viewport";
    } else if (id == EVT_VIEW_MENU_PROPERTIES) {
        name = "properties";
    }

    wxAuiPaneInfo& paneInfo = m_mgr.GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_mgr.Update();
}

void ProjectWindow::OnViewportSettings(wxCommandEvent&)
{
    ViewportSettingsDialog dlg(this, SceneViewportPane::Get(m_project).GetSettings(), m_project);
    dlg.ShowModal();
}
