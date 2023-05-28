#include <memory>

#include <wx/artprov.h>
#include <wx/msgdlg.h>
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
#include "pane/SelfOrganizingMapPane.hpp"

// FIXME Better solution
static bool isSOMInitialized = false;

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

    auto const& som = SelfOrganizingMap::Get(m_project);

    currIterations->Bind(wxEVT_UPDATE_UI, [&som](wxUpdateUIEvent& event) {
        if (isSOMInitialized) {
            event.SetText(wxString::Format("%d", som.GetIterations()));
            return;
        }
        event.SetText("");
    });

    currNbhdRadius->Bind(wxEVT_UPDATE_UI, [&som](wxUpdateUIEvent& event) {
        if (isSOMInitialized) {
            event.SetText(wxString::Format("%.6f", som.GetNeighborhood()));
            return;
        }
        event.SetText("");
    });

    currLearnRate->Bind(wxEVT_UPDATE_UI, [&som](wxUpdateUIEvent& event) {
        if (isSOMInitialized) {
            event.SetText(wxString::Format("%.6f", som.GetLearningRate()));
            return;
        }
        event.SetText("");
    });

    m_btnRun = group->AddButton("Run");
    m_btnRun->Disable();

    m_btnRun->Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
        if (SelfOrganizingMap::Get(m_project).IsTraining()) {
            event.SetText("Pause");
        } else {
            event.SetText("Run");
        }
    });

    m_btnRun->Bind(wxEVT_BUTTON, &SelfOrganizingMapPane::OnRun, this);

    // Texture mapping for the model
    m_btnTexmap = group->AddButton("Texture Mapping");
    m_btnTexmap->Bind(wxEVT_UPDATE_UI,
                      [this](wxUpdateUIEvent& event) { event.Enable(SelfOrganizingMap::Get(m_project).IsDone()); });
    m_btnTexmap->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { m_project.DoParameterization(); });
}

void SelfOrganizingMapPane::OnConfigure(wxCommandEvent&)
{
    SelfOrganizingMapDialog dlg(&ProjectWindow::Get(m_project), m_project);

    if (dlg.ShowModal() == wxID_OK) {
        auto const& modelID = dlg.GetModelID();
        auto const& mapID = dlg.GetMapID();

        for (auto const& o : ObjectList::Get(m_project)) {
            auto const& id = o->GetID();
            if (id == modelID) {
                m_project.theModel = o;
            }
        }

        if (auto it = m_project.theModel.lock()) {
            auto const& pos = it->GetPositions();
            m_project.theDataset = std::make_shared<Dataset<3>>(pos);
            it->SetViewFlags(ObjectViewFlag_Solid);
            log_info("Dataset count: %lu", pos.size());
        } else {
            log_fatal("Model is null");
            exit(EXIT_FAILURE);
        }

        auto& som = SelfOrganizingMap::Get(m_project);
        if (som.IsTraining()) {
            som.ToggleTraining();
        }
        som.StopWorker();
        som.CreateProcedure(m_project.theMap.lock(), m_project.theDataset);

        auto const& settings = ProjectSettings::Get(m_project);
        m_textModel->Clear();
        m_textMap->Clear();
        m_textIter->Clear();
        m_textRate->Clear();
        m_textRadius->Clear();
        *m_textModel << modelID;
        *m_textMap << mapID;
        *m_textIter << settings.GetMaxIterations();
        *m_textRate << settings.GetLearningRate();
        *m_textRadius << settings.GetNeighborhood();

        m_btnRun->Enable();

        isSOMInitialized = true;
    }
}

void SelfOrganizingMapPane::OnRun(wxCommandEvent&)
{
    SelfOrganizingMap::Get(m_project).ToggleTraining();
}
