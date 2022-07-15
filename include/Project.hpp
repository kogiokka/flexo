#ifndef WATERMARKING_PROJECT_H
#define WATERMARKING_PROJECT_H

#include <memory>

#include <wx/event.h>

#include "Lattice.hpp"
#include "ProjectSettings.hpp"
#include "SelfOrganizingMap.hpp"

class WatermarkingProject : public wxEvtHandler
{
public:
    WatermarkingProject();
    void Train();
    void Stop();
    bool IsTraining() const;
    bool IsDone() const;
    int GetIterations() const;
    float GetNeighborhood() const;

private:
    ProjectSettings m_conf;
    std::shared_ptr<Lattice> m_lattice;
    std::shared_ptr<SelfOrganizingMap> m_som;
};

#endif

