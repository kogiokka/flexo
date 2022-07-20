#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "InputData.hpp"
#include "Lattice.hpp"
#include "Mesh.hpp"
#include "OpenGLCanvas.hpp"
#include "Project.hpp"
#include "Renderer.hpp"
#include "SelfOrganizingMap.hpp"

class WatermarkingApp : public wxApp
{
    struct BoundingBox {
        glm::vec3 max;
        glm::vec3 min;
    };

    std::shared_ptr<WatermarkingProject> m_project;
    std::shared_ptr<Renderer> m_renderer;

    BoundingBox m_bbox;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    void ImportPolygonalModel(wxString const& path);
    void ImportVolumetricModel(wxString const& path);
    void SetCameraView(BoundingBox box);
    BoundingBox CalculateBoundingBox(std::vector<glm::vec3> positions);

    void OnLatticeMeshReady(wxCommandEvent& evt);
    void OnCmdStartTraining(wxCommandEvent& evt);
    void OnCmdStopTrainining(wxCommandEvent& evt);
    void OnCmdPauseTraining(wxCommandEvent& evt);
    void OnCmdToggleRenderOption(wxCommandEvent& evt);
    void OnCmdToggleLatticeFlag(wxCommandEvent& evt);
    void OnCmdDoWatermark(wxCommandEvent& evt);
    void OnCmdRebuildLatticeMesh(wxCommandEvent& evt);
    void OnMenuGenerateModel(wxCommandEvent& evt);
    void OnMenuImportModel(wxCommandEvent& evt);

private:
    wxDECLARE_EVENT_TABLE();
};

#endif
