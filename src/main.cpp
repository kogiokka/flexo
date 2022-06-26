#include <memory>

#include <wx/app.h>

#include "MainPanel.hpp"
#include "MainWindow.hpp"
#include "OpenGLCanvas.hpp"
#include "World.hpp"

class VolumetricModelWatermarking : public wxApp
{
public:
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
};

bool VolumetricModelWatermarking::OnInit()
{
    if (!wxApp::OnInit()) {
        return false;
    }

    float const ratio = static_cast<float>(world.pattern.width) / static_cast<float>(world.pattern.height);
    int latticeHeight = 128;
    int latticeWidth = static_cast<int>(latticeHeight * ratio);
    int iterationCap = 50000;
    float initLearningRate = 0.15f;

    world.lattice = std::make_unique<Lattice>(latticeWidth, latticeHeight);
    world.lattice->SetTrainingParameters(initLearningRate, iterationCap, LatticeFlags_CyclicNone);

    MainWindow* window = new MainWindow();

    MainPanel* panel = new MainPanel(window);
    panel->SetLattice(world.lattice);

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

void VolumetricModelWatermarking::OnUnhandledException()
{
    throw;
}

int VolumetricModelWatermarking::OnExit()
{
    return wxApp::OnExit();
}

IMPLEMENT_APP(VolumetricModelWatermarking)
