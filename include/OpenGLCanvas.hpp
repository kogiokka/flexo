#pragma once

#include "glad/glad.h"

#include "InputData.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "VertexArray.hpp"
#include "VolumeData.hpp"

#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>
#include <wx/string.h>

#include <memory>
#include <string>

class OpenGLCanvas : public wxGLCanvas
{
    bool isGLLoaded_;
    bool isAcceptingInput_;
    int iterPerFrame_;
    std::unique_ptr<wxGLContext> context_;
    std::unique_ptr<Lattice> lattice_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<InputData> inputs_;

public:
    OpenGLCanvas(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id = wxID_ANY,
                 wxPoint const& pos = wxDefaultPosition, wxSize const& size = wxDefaultSize, long style = 0,
                 wxString const& name = wxEmptyString);
    ~OpenGLCanvas();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void InitGL();
    void ResetCamera();
    void ResetLattice(int width, int height, int iterationCap, float initLearningRate);
    void ToggleRenderOption(RenderOption opt);
    void SetPlayOrPause(bool isAcceptingInput);
    void SetModelColorAlpha(float alpha);
    void SetIterationsPerFrame(int times);
    void OpenInputDataFile(wxString const& path);
    void UpdateLatticePositions();
    void UpdateLatticeFaces();
    int GetIterationCap() const;
    int GetLatticeWidth() const;
    int GetLatticeHeight() const;
    int GetCurrentIterations() const;
    int GetIterationsPerFrame() const;
    bool GetRenderOptionState(RenderOption opt) const;
    float GetInitialLearningRate() const;
    float GetModelTransparency() const;

    wxDECLARE_EVENT_TABLE();
};
