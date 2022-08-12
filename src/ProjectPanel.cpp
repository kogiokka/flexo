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
#include "Project.hpp"
#include "ProjectPanel.hpp"
#include "ProjectSettings.hpp"
#include "ProjectWindow.hpp"
#include "Renderer.hpp"
#include "World.hpp"

wxDEFINE_EVENT(EVT_TOGGLE_RENDER_FLAG, wxCommandEvent);
wxDEFINE_EVENT(EVT_TOGGLE_LATTICE_FLAG, wxCommandEvent);

wxBEGIN_EVENT_TABLE(ProjectPanel, wxPanel)
    EVT_TEXT(TE_LATTICE_WIDTH, ProjectPanel::OnSetLatticeWidth)
    EVT_TEXT(TE_LATTICE_HEIGHT, ProjectPanel::OnSetLatticeHeight)
    EVT_BUTTON(BTN_INITIALIZE_LATTICE, ProjectPanel::OnButtonInitializeLattice)
    EVT_BUTTON(BTN_WATERMARK, ProjectPanel::OnButtonWatermark)
    EVT_CHECKBOX(CB_RENDEROPT_MODEL, ProjectPanel::OnCheckboxModel)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_VERTEX, ProjectPanel::OnCheckboxLatticeVertex)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_EDGE, ProjectPanel::OnCheckboxLatticeEdge)
    EVT_CHECKBOX(CB_RENDEROPT_LAT_FACE, ProjectPanel::OnCheckboxLatticeFace)
    EVT_CHECKBOX(CB_RENDEROPT_LIGHT_SOURCE, ProjectPanel::OnCheckboxLightSource)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_X, ProjectPanel::OnCheckboxLatticeFlagsCyclicX)
    EVT_CHECKBOX(CB_LATTICE_FLAGS_CYCLIC_Y, ProjectPanel::OnCheckboxLatticeFlagsCyclicY)
    EVT_SLIDER(SLIDER_TRANSPARENCY, ProjectPanel::OnSliderTransparency)
    EVT_UPDATE_UI(PANEL_LATTICE, ProjectPanel::OnUpdateUI)
    EVT_UPDATE_UI(PANEL_WATERMARKING, ProjectPanel::OnUpdateUI)
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

    isLatticeInitialized = false;
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
    auto panel1 = CreateScrolledPanel(page, PANEL_LATTICE);
    auto panel3 = CreateScrolledPanel(page, PANEL_WATERMARKING);

    panel3->Disable();

    m_auiManager.SetManagedWindow(page);

    // Lattice (Map)
    {
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        auto row2 = new wxBoxSizer(wxHORIZONTAL);
        auto row3 = new wxBoxSizer(wxHORIZONTAL);
        auto row4 = new wxBoxSizer(wxHORIZONTAL);

        wxIntegerValidator<int> validDimen;
        validDimen.SetRange(1, 512);

        m_tcLatticeWidth = new wxTextCtrl(panel1, TE_LATTICE_WIDTH, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                          wxTE_CENTER, validDimen);
        m_tcLatticeHeight = new wxTextCtrl(panel1, TE_LATTICE_HEIGHT, wxEmptyString, wxDefaultPosition, wxDefaultSize,
                                           wxTE_CENTER, validDimen);
        *m_tcLatticeWidth << Lattice::Get(m_project).GetWidth();
        *m_tcLatticeHeight << Lattice::Get(m_project).GetHeight();

        auto chkBox1 = new wxCheckBox(panel1, CB_LATTICE_FLAGS_CYCLIC_X, "Cyclic on X");
        auto chkBox2 = new wxCheckBox(panel1, CB_LATTICE_FLAGS_CYCLIC_Y, "Cyclic on Y");
        chkBox1->SetValue(false);
        chkBox2->SetValue(false);

        row1->Add(new wxStaticText(panel1, wxID_ANY, "Lattice Dimensions: "), 1,
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeWidth, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row2->Add(m_tcLatticeHeight, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox1, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row3->Add(chkBox2, 1, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);
        row4->Add(new wxButton(panel1, BTN_INITIALIZE_LATTICE, "Initialize"),
                  wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

        layout->Add(row1, 0, wxGROW | wxALL, 5);
        layout->Add(row2, 0, wxGROW | wxALL, 5);
        layout->Add(row3, 0, wxGROW | wxALL, 5);
        layout->Add(row4, 0, wxGROW | wxALL, 5);
        panel1->SetSizer(layout);
    }

    // Watermarking
    {
        wxBoxSizer* layout = new wxBoxSizer(wxVERTICAL);
        auto row1 = new wxBoxSizer(wxHORIZONTAL);
        row1->Add(new wxButton(panel3, BTN_WATERMARK, "Watermark"), 1, wxGROW | wxLEFT | wxRIGHT, 5);
        layout->Add(row1, 0, wxGROW | wxALL, 5);
        panel3->SetSizer(layout);
    }

    m_somPanel = new SelfOrganizingMapPanel(page, PANEL_SOM, wxDefaultPosition, wxDefaultSize, m_project);

    wxAuiPaneInfo info = wxAuiPaneInfo().Center().CloseButton(false);
    m_auiManager.AddPane(panel1, info.Name("lattice").Caption("Lattice"));
    m_auiManager.AddPane(m_somPanel, info.Name("som").Caption("SOM"));
    m_auiManager.AddPane(panel3, info.Name("watermark").Caption("Watermarking"));
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

void ProjectPanel::OnButtonInitializeLattice(wxCommandEvent&)
{
    m_project.InitializeLattice();
    m_project.UpdateLatticeGraphics();
    isLatticeInitialized = true;
}
void ProjectPanel::OnButtonWatermark(wxCommandEvent&)
{
    m_project.DoWatermark();
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

void ProjectPanel::OnCheckboxLatticeFlagsCyclicX(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicX);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnCheckboxLatticeFlagsCyclicY(wxCommandEvent&)
{
    wxCommandEvent event(EVT_TOGGLE_LATTICE_FLAG, GetId());
    event.SetInt(LatticeFlags_CyclicY);
    ProcessWindowEvent(event);
}

void ProjectPanel::OnSliderTransparency(wxCommandEvent&)
{
    float const range = static_cast<float>(m_slider->GetMax() - m_slider->GetMin());
    float const value = static_cast<float>(m_slider->GetValue());
    world.modelColorAlpha = (range - value) / range;
}

void ProjectPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
    switch (event.GetId()) {
    case PANEL_LATTICE:
        event.Enable(m_somPanel->IsProjectStopped() && !SelfOrganizingMap::Get(m_project).IsTraining());
        break;
    case PANEL_WATERMARKING:
        event.Enable(SelfOrganizingMap::Get(m_project).IsDone());
        break;
    default:
        break;
    }
}

void ProjectPanel::OnSetLatticeWidth(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetWidth(tmp);
    }
}

void ProjectPanel::OnSetLatticeHeight(wxCommandEvent& event)
{
    long tmp;
    if (event.GetString().ToLong(&tmp)) {
        Lattice::Get(m_project).SetHeight(tmp);
    }
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
