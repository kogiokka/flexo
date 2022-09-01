#ifndef WATERMARKING_APP_H
#define WATERMARKING_APP_H

#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "Project.hpp"

class WatermarkingApp : public wxApp
{
public:
    WatermarkingApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;

private:
    std::shared_ptr<WatermarkingProject> m_project;
};

#endif
