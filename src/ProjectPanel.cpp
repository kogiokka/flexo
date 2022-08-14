#include <array>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/event.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "Lattice.hpp"
#include "LatticePanel.hpp"
#include "Project.hpp"
#include "ProjectPanel.hpp"
#include "ProjectSettings.hpp"
#include "ProjectWindow.hpp"
#include "Renderer.hpp"
#include "WatermarkingPanel.hpp"
#include "World.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);

wxBEGIN_EVENT_TABLE(ProjectPanel, wxPanel)
    EVT_CHECKBOX(CB_RENDEROPT_MODEL, ProjectPanel::OnCheckboxModel)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, ProjectPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, ProjectPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, ProjectPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, ProjectPanel::OnCheckboxLightSource)
    EVT_SLIDER(SLIDER_TRANSPARENCY, ProjectPanel::OnSliderTransparency)
wxEND_EVENT_TABLE()

// Register factory: ProjectPanel
static WatermarkingProject::AttachedWindows::RegisteredFactory const factoryKey {
    [](WatermarkingProject& project) -> wxWeakRef<wxWindow> {
        auto& window = ProjectWindow::Get(project);
        wxWindow* mainPage = window.GetMainPage();
        wxASSERT(mainPage != nullptr);

        auto panel = new ProjectPanel(mainPage, PANEL_NOTEBOOK, wxDefaultPosition, wxDefaultSize, project);
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

    m_somPanel = new SelfOrganizingMapPanel(page, PANEL_SOM, wxDefaultPosition, wxDefaultSize, m_project);
    auto latticePanel = new LatticePanel(page, PANEL_LATTICE, wxDefaultPosition, wxDefaultSize, m_project);
    auto watermarking = new WatermarkingPanel(page, PANEL_WATERMARKING, wxDefaultPosition, wxDefaultSize, m_project);
    watermarking->Disable();

    wxAuiPaneInfo info = wxAuiPaneInfo().Center().CloseButton(false);
    m_auiManager.AddPane(latticePanel, info.Name("lattice").Caption("Lattice"));
    m_auiManager.AddPane(m_somPanel, info.Name("som").Caption("SOM"));
    m_auiManager.AddPane(watermarking, info.Name("watermark").Caption("Watermarking"));
    m_auiManager.Update();

    AddPage(page, "Project");
}

void ProjectPanel::PopulateRenderingPage()
{
    wxPanel* page = new wxPanel(this, wxID_ANY);

    auto row1 = new wxBoxSizer(wxVERTICAL);
    auto row2 = new wxBoxSizer(wxVERTICAL);
    auto chkBox1 = new wxCheckBox(page, CB_RENDEROPT_MODEL, "Model");
    auto chkBox2 = new wxCheckBox(page, CB_RENDEROPT_LAT_VERTEX, "Lattice Vertex");
    auto chkBox3 = new wxCheckBox(page, CB_RENDEROPT_LAT_EDGE, "Lattice Edge");
    auto chkBox4 = new wxCheckBox(page, CB_RENDEROPT_LAT_FACE, "Lattice Face");
    auto chkBox5 = new wxCheckBox(page, CB_RENDEROPT_LIGHT_SOURCE, "Light Source");
    chkBox1->SetValue(true);
    chkBox2->SetValue(true);
    chkBox3->SetValue(true);
    chkBox4->SetValue(false);
    chkBox5->SetValue(false);

    // Trigger the event to update render flags.
    wxCommandEvent event;
    OnCheckboxModel(event);
    OnCheckboxLatticeVertex(event);
    OnCheckboxLatticeEdge(event);

    auto surfAlphaLabel = new wxStaticText(page, wxID_ANY, "Model Transparency (%)");
    int const sliderInit = static_cast<int>(100.0f - world.modelColorAlpha * 100.0f);
    m_slider = new wxSlider(page, SLIDER_TRANSPARENCY, sliderInit, 0, 100, wxDefaultPosition, wxDefaultSize,
                            wxSL_HORIZONTAL | wxSL_LABELS | wxSL_INVERSE);
    row1->Add(chkBox1, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox2, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox3, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox4, 0, wxGROW | wxALL, 5);
    row1->Add(chkBox5, 0, wxGROW | wxALL, 5);
    row2->Add(surfAlphaLabel, 0, wxGROW);
    row2->Add(m_slider, 0, wxGROW);

    wxBoxSizer* boxLayout = new wxBoxSizer(wxVERTICAL);
    boxLayout->Add(row1, 0, wxGROW | wxALL, 10);
    boxLayout->Add(row2, 0, wxGROW | wxALL, 10);
    page->SetSizer(boxLayout);

    AddPage(page, "Rendering");
}

void ProjectPanel::OnCheckboxModel(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawModel);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeVertex(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeVertex);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeEdge(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeEdge);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeFace(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLatticeFace);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLightSource(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_RENDER_FLAG, GetId());
    event.SetInt(RenderFlag_DrawLightSource);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
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
        name = "watermark";
    }

    wxAuiPaneInfo& paneInfo = m_auiManager.GetPane(name);
    if (paneInfo.IsShown()) {
        paneInfo.Hide();
    } else {
        paneInfo.Show();
    }

    m_auiManager.Update();
}
