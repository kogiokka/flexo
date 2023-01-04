#ifndef FLEXO_APP_H
#define FLEXO_APP_H

#include <vector>

#include <glm/glm.hpp>
#include <wx/app.h>

#include "Project.hpp"

class FlexoApp : public wxApp
{
public:
    FlexoApp();
    virtual bool OnInit() override;
    virtual int OnExit() override;
    virtual void OnUnhandledException() override;

private:
    std::shared_ptr<FlexoProject> m_project;
};

#endif
