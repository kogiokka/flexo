#ifndef SCENE_VIEWPORT_PANE_H
#define SCENE_VIEWPORT_PANE_H

#include <memory>
#include <tuple>

#include <glad/glad.h> // before <wx/glcanvas.h>

#include <glm/glm.hpp>
#include <wx/dcclient.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/glcanvas.h>
#include <wx/string.h>

#include "gfx/Graphics.hpp"

class WatermarkingProject;

class SceneViewportPane : public wxGLCanvas
{
public:
    static SceneViewportPane& Get(WatermarkingProject& project);
    static SceneViewportPane const& Get(WatermarkingProject const& project);

    SceneViewportPane(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id, wxPoint const& pos,
                      wxSize const& size, WatermarkingProject& project);
    ~SceneViewportPane();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void InitGL();
    void ResetCamera();

private:
    WatermarkingProject& m_project;
    GLWRPtr<IGLWRRenderTarget> m_renderTarget;
    std::unique_ptr<wxGLContext> m_context;
    bool m_isGLLoaded;
    float m_rateMove;
    float m_rateRotate;
    int m_dirHorizontal;
    std::tuple<int, int, float, float> m_originRotate;
    std::tuple<float, float, glm::vec3> m_originTranslate;

    inline float RoundGuard(float radian);

    wxDECLARE_EVENT_TABLE();
};

#endif
