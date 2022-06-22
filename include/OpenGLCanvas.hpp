#pragma once

#include "glad/glad.h"
#include "InputData.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
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
    std::unique_ptr<wxGLContext> context_;
    std::unique_ptr<Renderer> renderer_;

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
    void ResetLattice();
    void ToggleRenderOption(RenderOption opt);
    void SetModelColorAlpha(float alpha);
    void OpenInputDataFile(wxString const& path);
    bool GetRenderOptionState(RenderOption opt) const;
    float GetModelTransparency() const;

private:
    void UpdateScene();
    void UpdateLatticeEdges();
    void BuildLatticeMesh();

    wxDECLARE_EVENT_TABLE();
};
