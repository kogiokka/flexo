#ifndef OPENGL_CANVAS_H
#define OPENGL_CANVAS_H

#include <memory>
#include <tuple>

#include <glad/glad.h> // before <wx/glcanvas.h>

#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>
#include <wx/string.h>

#include "Renderer.hpp"

class OpenGLCanvas : public wxGLCanvas
{
    bool isGLLoaded_;
    std::unique_ptr<wxGLContext> context_;
    std::unique_ptr<Renderer> renderer_;
    float rateMove_;
    float rateRotate_;
    int dirHorizontal_;
    std::tuple<int, int, float, float> originRotate_;
    std::tuple<float, float, glm::vec3> originTranslate_;

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
    inline float RoundGuard(float radian);

    wxDECLARE_EVENT_TABLE();
};

#endif
