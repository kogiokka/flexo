#ifndef OPENGL_CANVAS_H
#define OPENGL_CANVAS_H

#include <memory>
#include <tuple>

#include <glad/glad.h> // before <wx/glcanvas.h>

#include <wx/dcclient.h>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>
#include <wx/string.h>
#include <wx/timer.h>

#include "Renderer.hpp"

class OpenGLCanvas : public wxGLCanvas
{
    bool m_isGLLoaded;
    std::unique_ptr<wxGLContext> m_context;
    std::unique_ptr<Renderer> m_renderer;
    float m_rateMove;
    float m_rateRotate;
    int m_dirHorizontal;
    std::tuple<int, int, float, float> m_originRotate;
    std::tuple<float, float, glm::vec3> m_originTranslate;
    wxTimer* m_updateTimer;

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
    void OnUpdateTimer(wxTimerEvent& event);
    void InitGL();
    void ResetCamera();
    void ResetLattice();
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
