#include <memory>

#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "WatermarkingApp.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "pane/PropertiesPane.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "pane/TextureMappingPane.hpp"
#include "util/Logger.h"

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
    auto* mapping = new TextureMappingPane(page, project);
    auto* properties = new PropertiesPane(page, project);
    mapping->Disable();

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
    mgr.AddPane(properties,
                wxAuiPaneInfo()
                    .Name("properties")
                    .Caption("Properties")
                    .Right()
                    .Layer(1)
                    .CloseButton(true)
                    .MaximizeButton(true)
                    .MinSize(minSize));
    mgr.AddPane(
        som,
        wxAuiPaneInfo().Name("som").Caption("SOM").Right().Layer(1).CloseButton(true).MaximizeButton(true).MinSize(
            minSize));
    mgr.AddPane(mapping,
                wxAuiPaneInfo()
                    .Name("mapping")
                    .Caption("Texture Mapping")
                    .Right()
                    .Layer(1)
                    .CloseButton(true)
                    .MaximizeButton(true)
                    .MinSize(minSize)
                    .Hide());
    mgr.Update();

    wxUpdateUIEvent::SetUpdateInterval(16);

    m_project->CreateScene();

    return true;
}
