#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "Project.hpp"
#include "SelfOrganizingMap.hpp"
#include "pane/TextureMappingPane.hpp"

TextureMappingPane::TextureMappingPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
{
    auto* group = AddGroup("Control", 1);
    auto* btn = group->AddButton("Parameterize");

    Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });
    btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { m_project.DoParameterization(); });
}
