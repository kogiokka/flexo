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

#include "Overlays.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"
#include "object/Map.hpp"

class FlexoProject;
class Scene;

class SceneViewportPane : public wxGLCanvas
{
public:
    static SceneViewportPane& Get(FlexoProject& project);
    static SceneViewportPane const& Get(FlexoProject const& project);

    struct Settings {
        Color background;
        OverlayFlags overlayFlags;
    };

    SceneViewportPane(wxWindow* parent, wxGLAttributes const& dispAttrs, wxWindowID id, wxPoint const& pos,
                      wxSize const& size, FlexoProject& project);
    ~SceneViewportPane();
    void InitGL();
    void ResetCamera();
    Graphics& GetGL();
    Settings GetSettings() const;
    void SetCurrentMap(std::weak_ptr<Map<3, 2>> map);
    void SetScene(Scene const& scene);

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnMouseLeftDown(wxMouseEvent& event);
    void OnMouseRightDown(wxMouseEvent& event);
    void OnMenuScreenshot(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    Camera CreateDefaultCamera() const;
    inline float RoundGuard(float radian);
    void InitFrame(Graphics& gfx);

    bool m_isGLLoaded;
    int m_dirHorizontal;
    std::tuple<int, int, float, float> m_originRotate;
    std::tuple<float, float, glm::vec3> m_originTranslate;
    GLWRPtr<IGLWRRenderTargetView> m_rtv;
    GLWRPtr<IGLWRDepthStencilView> m_dsv;
    std::unique_ptr<wxGLContext> m_context;
    std::shared_ptr<Graphics> m_gfx;
    std::shared_ptr<Renderer> m_renderer;

    std::unique_ptr<Overlays> m_overlays;
    Settings m_settings;
    std::weak_ptr<Map<3, 2>> m_currMap;
    Scene const* m_scene;

    FlexoProject& m_project;

    wxDECLARE_EVENT_TABLE();
};

#endif
