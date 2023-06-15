#include <memory>

#include <wx/artprov.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "dialog/SelfOrganizingMapDialog.hpp"
#include "event/SliderFloatEvent.hpp"
#include "object/Map.hpp"
#include "object/ObjectList.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

SelfOrganizingMapPane::SelfOrganizingMapPane(wxWindow* parent, FlexoProject& project)
    : ControlsPaneBase(parent, project)
{
    PopulateConfigPane();
    PopulateTrainingPane();
}

void SelfOrganizingMapPane::PopulateConfigPane()
{
    auto* group = AddGroup("Configuration", 6);
    m_textModel = group->AddReadOnlyText("Model");
    m_textMap = group->AddReadOnlyText("Map");
    m_textIter = group->AddReadOnlyText("Max Iterations");
    m_textRate = group->AddReadOnlyText("Initial Leanring Rate");
    m_textRadius = group->AddReadOnlyText("Initial Neighborhood Radius");
    m_btnConfig = group->AddButton("Configure");

    m_btnConfig->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnConfigure, this);
}

void SelfOrganizingMapPane::PopulateTrainingPane()
{
    auto* group = AddGroup("Training", 7);

    auto* currIterations = group->AddReadOnlyText("Iterations");
    auto* currNbhdRadius = group->AddReadOnlyText("Neighborhood Radius");
    auto* currLearnRate = group->AddReadOnlyText("Leanring Rate");

    currIterations->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_som) {
            event.SetText(wxString::Format("%d", m_som->GetIterations()));
            return;
        }
        event.SetText("");
    });

    currNbhdRadius->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_som) {
            event.SetText(wxString::Format("%.6f", m_som->GetNeighborhood()));
            return;
        }
        event.SetText("");
    });

    currLearnRate->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (m_som) {
            event.SetText(wxString::Format("%.6f", m_som->GetLearningRate()));
            return;
        }
        event.SetText("");
    });

    m_btnRun = group->AddButton("Run");

    m_btnRun->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (!m_som) {
            return;
        }
        if (m_som->IsDone()) {
            event.SetText("Run");
            event.Enable(false);
        } else if (m_som->IsTraining()) {
            event.SetText("Pause");
        } else {
            event.SetText("Run");
        }
    });

    m_btnRun->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnRun, this);

    // Texture mapping for the model
    m_btnTexmap = group->AddButton("Texture Mapping");
    m_btnTexmap->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) { event.Enable(m_som && m_som->IsDone()); });
    m_btnTexmap->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnParameterization, this);
}

void SelfOrganizingMapPane::OnConfigure(wxCommandEvent&)
{
    SelfOrganizingMapDialog dlg(m_project.GetWindow(), m_project);

    if (dlg.ShowModal() == wxID_OK) {
        m_somModel = std::make_unique<SelfOrganizingMapModel<3, 2>>(dlg.GetConfig());

        m_textModel->Clear();
        m_textMap->Clear();
        m_textIter->Clear();
        m_textRate->Clear();
        m_textRadius->Clear();
        *m_textModel << m_somModel->object->GetID();
        *m_textMap << m_somModel->map->GetID();
        *m_textIter << static_cast<long int>(m_somModel->maxSteps);
        *m_textRate << m_somModel->learningRate;
        *m_textRadius << m_somModel->neighborhood;

        m_som = std::make_unique<SelfOrganizingMap>(*m_somModel, m_project);

        SceneViewportPane::Get(m_project).SetCurrentMap(m_somModel->map);

        m_btnRun->Enable(true);
    }
}

void SelfOrganizingMapPane::OnRun(wxCommandEvent&)
{
    if (!m_som) {
        return;
    }

    m_som->ToggleTraining();
}

void SelfOrganizingMapPane::OnParameterization(wxCommandEvent&)
{
    auto model = std::dynamic_pointer_cast<SurfaceVoxels>(m_somModel->object);
    if (!model) {
        wxMessageDialog dlg(m_project.GetWindow(), "Not a volumetric model!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    float progress;
    auto status = model->Parameterize(*(m_somModel->map), progress);

    wxProgressDialog dialog("Parameterizing", "Please wait...", 100, m_project.GetWindow(),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    model->SetTexture(m_somModel->map->GetTexture());

    // After the parametrization done, regenerate the drawables to update texture coordinates.
    model->SetViewFlags(ObjectViewFlag_Textured);
    model->GenerateMesh();
    model->GenerateDrawables(SceneViewportPane::Get(m_project).GetGL());
}
