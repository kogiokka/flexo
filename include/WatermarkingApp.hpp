#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "Project.hpp"

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

    void OnMenuAddModel(wxCommandEvent& event);
    void OnMenuImportModel(wxCommandEvent& event);

private:
    wxString CreateDatasetLabel(std::string const& name, unsigned int id) const

    wxDECLARE_EVENT_TABLE();
};

#endif
