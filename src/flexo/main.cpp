#include <memory>

#include "FlexoApp.hpp"
#include "ProjectWindow.hpp"
#include "SceneController.hpp"
#include "SelfOrganizingMap.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "log/Logger.h"
#include "pane/PropertiesPane.hpp"
#include "pane/SceneOutlinerPane.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

wxIMPLEMENT_APP(FlexoApp);

FlexoApp::FlexoApp()
    : m_project(nullptr)
{
}

void FlexoApp::OnUnhandledException()
{
    throw;
}

int FlexoApp::OnExit()
{
    return wxApp::OnExit();
}

bool FlexoApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    m_project = std::make_shared<FlexoProject>();
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
    auto* properties = new PropertiesPane(page, project);

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
    mgr.Update();

    wxUpdateUIEvent::SetUpdateInterval(16);

    SceneController::Get(project).CreateScene();

    return true;
}
