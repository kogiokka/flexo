#include <memory>

#include "Mesh.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "pane/WatermarkingPane.hpp"

wxIMPLEMENT_APP(WatermarkingApp);

WatermarkingApp::WatermarkingApp()
    : m_project(nullptr)
{
}

void WatermarkingApp::OnUnhandledException()
{
    throw;
}

int WatermarkingApp::OnExit()
{
    SelfOrganizingMap::Get(*m_project).StopWorker();
    return wxApp::OnExit();
}

bool WatermarkingApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    STLImporter stlImp;
    world.uvsphere = stlImp.ReadFile("res/models/UVSphere.stl");
    world.cube = stlImp.ReadFile("res/models/cube.stl");

    m_project = std::make_shared<WatermarkingProject>();
    auto& project = *m_project;
    auto& window = ProjectWindow::Get(project);
    auto& viewport = SceneViewportPane::Get(project);

    window.Show();
    window.Maximize();
    viewport.InitGL();
    viewport.SetFocus();

    auto& mgr = window.GetPaneManager();
    auto page = window.GetMainPage();

    auto* outliner = new SceneOutlinerPane(page, project);
    auto* som = new SelfOrganizingMapPane(page, project);
    auto* watermarking = new WatermarkingPane(page, project);
    watermarking->Disable();

    wxSize const minSize = page->FromDIP(wxSize(450, 20));
    mgr.AddPane(&viewport,
                wxAuiPaneInfo()
                    .Name("viewport")
                    .Caption("3D Viewport")
                    .Center()
                    .CloseButton(true)
                    .MaximizeButton(true)
                    .MinSize(minSize));
    mgr.AddPane(outliner,
                wxAuiPaneInfo()
                    .Name("outliner")
                    .Caption("Scene Outliner")
                    .Right()
                    .Layer(1)
                    .CloseButton(true)
                    .MaximizeButton(true)
                    .MinSize(minSize));
    mgr.AddPane(
        som,
        wxAuiPaneInfo().Name("som").Caption("SOM").Right().Layer(1).CloseButton(true).MaximizeButton(true).MinSize(
            minSize));
    mgr.AddPane(watermarking,
                wxAuiPaneInfo()
                    .Name("watermarking")
                    .Caption("Watermarking")
                    .Right()
                    .Layer(1)
                    .CloseButton(true)
                    .MaximizeButton(true)
                    .MinSize(minSize)
                    .Hide());
    mgr.Update();

    wxUpdateUIEvent::SetUpdateInterval(16);

    return true;
}
