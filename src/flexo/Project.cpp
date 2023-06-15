#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/valnum.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "EditableMesh.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SceneController.hpp"
#include "SolidDrawable.hpp"
#include "TexturedDrawable.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "WireDrawable.hpp"
#include "log/Logger.h"
#include "object/SurfaceVoxels.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/TextureWidget.hpp"

FlexoProject::FlexoProject()
    : m_frame {}
    , m_panel {}
    , theDataset(nullptr)
    , theMap()
    , theModel()
{
}

FlexoProject::~FlexoProject()
{
}

void FlexoProject::SetFrame(wxFrame* frame)
{
    m_frame = frame;
}

void FlexoProject::SetPanel(wxWindow* panel)
{
    m_panel = panel;
}

wxWindow* FlexoProject::GetPanel()
{
    return m_panel;
}

void FlexoProject::DoParameterization()
{
    auto model = std::dynamic_pointer_cast<SurfaceVoxels>(theModel.lock());
    if (!model) {
        wxMessageDialog dlg(&ProjectWindow::Get(*this), "Not a volumetric model!", "Error", wxCENTER | wxICON_ERROR);
        dlg.ShowModal();
        return;
    }

    float progress;
    auto status = model->Parameterize(*(theMap.lock()), progress);

    wxProgressDialog dialog("Parameterizing", "Please wait...", 100, &ProjectWindow::Get(*this),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    if (auto map = theMap.lock()) {
        model->SetTexture(map->GetTexture());
    }

    // After the parametrization done, regenerate the drawables to update texture coordinates.
    model->SetViewFlags(ObjectViewFlag_Textured);
    model->GenerateMesh();
    model->GenerateDrawables(SceneViewportPane::Get(*this).GetGL());
}

void FlexoProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    auto model = std::make_shared<SurfaceVoxels>(data);
    model->SetViewFlags(ObjectViewFlag_Solid);
    model->SetTexture(Bind::TextureManager::Resolve(SceneViewportPane::Get(*this).GetGL(), "images/blank.png", 0));

    log_info("%lu voxels will be rendered.", model->Voxels().size());

    theModel = model;

    SceneController::Get(*this).AcceptObject(theModel.lock());
}
