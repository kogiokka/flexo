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
    void OnMenuScreenshot(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void InitGL();
    void ResetCamera();

private:
    Camera CreateDefaultCamera() const;
    float cameraZoomStep(float zoom) const;

    WatermarkingProject& m_project;
    GLWRPtr<IGLWRRenderTargetView> m_rtv;
    GLWRPtr<IGLWRDepthStencilView> m_dsv;
    std::unique_ptr<wxGLContext> m_context;
    bool m_isGLLoaded;
    float m_rateMove;
    float m_rateRotate;
    int m_dirHorizontal;
    std::tuple<int, int, float, float> m_originRotate;
    std::tuple<float, float, glm::vec3> m_originTranslate;

    inline float RoundGuard(float radian);
    void InitFrame(Graphics& gfx);

    wxDECLARE_EVENT_TABLE();
};

#endif
