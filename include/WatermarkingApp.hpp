#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "InputData.hpp"
#include "Lattice.hpp"
#include "OpenGLCanvas.hpp"
#include "Project.hpp"
#include "SelfOrganizingMap.hpp"

class WatermarkingApp : public wxApp
{
    std::shared_ptr<WatermarkingProject> m_project;

public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;
    void ImportPolygonalModel(wxString const& path);
    void ImportVolumetricModel(wxString const& path);
    BoundingBox CalculateBoundingBox(std::vector<glm::vec3> positions);

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

