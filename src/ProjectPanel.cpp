#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>

#include "LatticePanel.hpp"
#include "Project.hpp"
#include "ProjectPanel.hpp"
#include "ProjectWindow.hpp"
#include "SceneOutlinerPanel.hpp"
#include "SelfOrganizingMapPanel.hpp"
#include "WatermarkingPanel.hpp"
#include "common/Logger.hpp"

// Register factory: ProjectPanel
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        auto panel = new ProjectPanel(mainPage, Panel_Notebook, wxDefaultPosition, wxDefaultSize, project);
        project.SetPanel(panel);
        return panel;
    }
};

ProjectPanel& ProjectPanel::Get(WatermarkingProject& project)
{
    return project.AttachedWindows::Get<ProjectPanel>(factoryKey);
}

ProjectPanel const& ProjectPanel::Get(WatermarkingProject const& project)
{
    return Get(const_cast<WatermarkingProject&>(project));
}

ProjectPanel::ProjectPanel(wxWindow* parent, wxWindowID id, wxPoint const& pos, wxSize const& size,
                           WatermarkingProject& project)
    : wxNotebook(parent, id, pos, size)
    , m_project(project)
{
    PopulateProjectPage();
    PopulateRenderingPage();

    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_LATTICE);
    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_SOM);
    m_project.Bind(wxEVT_MENU, &ProjectPanel::OnTogglePane, this, EVT_VIEW_MENU_WATERMARKING);
}

ProjectPanel::~ProjectPanel()
{
}

wxWindow* ProjectPanel::CreateScrolledPanel(wxWindow* parent, wxWindowID winid)
{
    auto panel = new wxScrolledWindow(parent, winid, wxDefaultPosition, wxDefaultSize);
    panel->SetScrollRate(10, 10);
    return panel;
}

void ProjectPanel::PopulateProjectPage()
{
    auto page = CreateScrolledPanel(this, wxID_ANY);
    m_auiManager.SetManagedWindow(page);

    auto lattice = new LatticePanel(page, m_project);
    auto som = new SelfOrganizingMapPanel(page, m_project);
    auto watermarking = new WatermarkingPanel(page, m_project);
    watermarking->Disable();

    wxAuiPaneInfo info = wxAuiPaneInfo().Center().CloseButton(false);
    m_auiManager.AddPane(lattice, info.Name("lattice").Caption("Lattice"));
    m_auiManager.AddPane(som, info.Name("som").Caption("SOM"));
    m_auiManager.AddPane(watermarking, info.Name("watermarking").Caption("Watermarking"));
    m_auiManager.Update();

    AddPage(page, "Project");
}

void ProjectPanel::PopulateRenderingPage()
{
    auto page = CreateScrolledPanel(this, wxID_ANY);
    auto layout = new wxBoxSizer(wxVERTICAL);
    auto sceneOutliner = new SceneOutlinerPanel(page, m_project);
    layout->Add(sceneOutliner, wxSizerFlags().Expand().Proportion(1));
    page->SetSizer(layout);

    AddPage(page, "Rendering");
}

void ProjectPanel::OnTogglePane(wxCommandEvent& event)
{
    wxString name;
    int const id = event.GetId();

    if (id == EVT_VIEW_MENU_LATTICE) {
        name = "lattice";
    } else if (id == EVT_VIEW_MENU_SOM) {
        name = "som";
    } else if (id == EVT_VIEW_MENU_WATERMARKING) {
        name = "watermarking";
    }

    wxAuiPaneInfo& paneInfo = m_auiManager.GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_auiManager.Update();
}
