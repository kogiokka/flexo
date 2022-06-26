#include <memory>

#include "MainPanel.hpp"
#include "MainWindow.hpp"
#include "OpenGLCanvas.hpp"
#include "WatermarkingApp.hpp"
#include "World.hpp"

WatermarkingApp::WatermarkingApp()
{
    float const ratio = static_cast<float>(world.pattern.width) / static_cast<float>(world.pattern.height);
    m_conf.height = 128;
    m_conf.width = static_cast<int>(m_conf.height * ratio);
    m_conf.maxIterations = 50000;
    m_conf.initialRate = 0.15f;
}

TrainingConfig& WatermarkingApp::GetTrainingConfig()
{
    return m_conf;
}

void WatermarkingApp::ToggleLatticeFlags(LatticeFlags flag)
{
    m_conf.flags ^= flag;
}

void WatermarkingApp::CreateLattice()
{
    world.lattice = std::make_unique<Lattice>(m_conf.width, m_conf.height);
    world.lattice->SetTrainingParameters(m_conf.initialRate, m_conf.maxIterations, m_conf.flags);
    world.lattice->Train(*world.dataset);
}

bool WatermarkingApp::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    // FIXME
    world.lattice = std::make_unique<Lattice>(m_conf.width, m_conf.height);
    world.lattice->SetTrainingParameters(m_conf.initialRate, m_conf.maxIterations, m_conf.flags);

    MainWindow* window = new MainWindow();
    MainPanel* panel = new MainPanel(window);

    wxGLAttributes attrs;
    attrs.PlatformDefaults().MinRGBA(8, 8, 8, 8).DoubleBuffer().Depth(24).EndList();
    auto canvas = new OpenGLCanvas(window, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    canvas->SetFocus();

    panel->SetOpenGLCanvas(canvas);
    window->SetMainPanel(panel);
    window->SetOpenGLCanvas(canvas);

    window->Show();

    canvas->InitGL();

    return true;
}

void WatermarkingApp::OnUnhandledException()
{
    throw;
}

int WatermarkingApp::OnExit()
{
    return wxApp::OnExit();
}

wxIMPLEMENT_APP(WatermarkingApp);
