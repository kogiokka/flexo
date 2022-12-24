#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>
#include <wx/progdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "EditableMesh.hpp"
#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "TransformStack.hpp"
#include "VecUtil.hpp"
#include "VolumetricModelData.hpp"
#include "World.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/ObjectList.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/Technique.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"
#include "object/Cube.hpp"
#include "object/Guides.hpp"
#include "object/Sphere.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/SelfOrganizingMapPane.hpp"
#include "util/Logger.h"

#include "gfx/TechniqueManager.hpp"
#include "gfx/bindable/InputLayout.hpp"
#include "gfx/bindable/Primitive.hpp"
#include "gfx/bindable/RasterizerState.hpp"
#include "gfx/bindable/UniformBuffer.hpp"
#include "gfx/bindable/VertexBuffer.hpp"
#include "gfx/bindable/program/FragmentShaderProgram.hpp"
#include "gfx/bindable/program/VertexShaderProgram.hpp"

void InitRenderingTechniques(Graphics& gfx);

WatermarkingProject::WatermarkingProject()
    : m_frame {}
    , m_panel {}
    , m_model(nullptr)
{
    m_imageFile = "res/images/mandala.png";
    Bind(EVT_ADD_UV_SPHERE, &WatermarkingProject::OnMenuAddModel, this);
    Bind(EVT_SOM_PANE_MAP_CHANGED, &WatermarkingProject::OnSOMPaneMapChanged, this);

    Bind(EVT_OPEN_IMAGE, [this](wxCommandEvent& event) {
        m_imageFile = event.GetString().ToStdString();

        auto& gfx = Graphics::Get(*this);
        auto& objlist = ObjectList::Get(*this);
        std::string const filename = event.GetString().ToStdString();

        if (m_model) {
            m_model->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            objlist.Add(ObjectType_Model, m_model);
            objlist.Submit(Renderer::Get(*this));
        }

        if (world.theMap) {
            world.theMap->SetTexture(Bind::TextureManager::Resolve(gfx, filename.c_str(), 0));
            objlist.Add(ObjectType_Map, world.theMap);
            objlist.Submit(Renderer::Get(*this));
        }
    });
}

WatermarkingProject::~WatermarkingProject()
{
}

void WatermarkingProject::CreateScene()
{
    InitRenderingTechniques(Graphics::Get(*this));
    auto& objlist = ObjectList::Get(*this);
    auto& renderer = Renderer::Get(*this);

    TransformStack ts;
    ts.PushTranslate(0.0f, 5.0f, 0.0f);
    ts.PushScale(0.2f, 0.2f, 0.2f);
    auto light = std::make_shared<Sphere>();
    light->SetTransform(ts);

    auto cube = std::make_shared<Cube>();
    cube->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));
    cube->SetViewFlags(ObjectViewFlag_Solid);
    objlist.Add(ObjectType_Cube, cube);
    objlist.Add(ObjectType_Light, light);
    objlist.Add(ObjectType_Guides, std::make_shared<Guides>());
    objlist.Submit(renderer);
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    m_model->SetViewFlags(ObjectViewFlag_Solid);
    ObjectList::Get(*this).Submit(Renderer::Get(*this));

    SelfOrganizingMap::Get(*this).CreateProcedure(world.theMap, world.theDataset);
}

void WatermarkingProject::StopProject()
{
    auto& som = SelfOrganizingMap::Get(*this);
    if (som.IsTraining()) {
        som.ToggleTraining();
    }
    som.StopWorker();
}

void WatermarkingProject::SetFrame(wxFrame* frame)
{
    m_frame = frame;
}

void WatermarkingProject::SetPanel(wxWindow* panel)
{
    m_panel = panel;
}

wxWindow* WatermarkingProject::GetPanel()
{
    return m_panel;
}

void WatermarkingProject::DoWatermark()
{
    float progress;
    auto status = m_model->Parameterize(*world.theMap, progress);

    wxProgressDialog dialog("Texture Mapping", "Please wait...", 100, &ProjectWindow::Get(*this),
                            wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_SMOOTH | wxPD_ESTIMATED_TIME);

    while (status.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready) {
        dialog.Update(static_cast<int>(progress));
    }

    // After the parametrization done, regenerate the drawables to update texture coordinates.
    m_model->GenerateDrawables(Graphics::Get(*this));

    m_model->SetViewFlags(ObjectViewFlag_Textured);
    ObjectList::Get(*this).Submit(Renderer::Get(*this));
}

void WatermarkingProject::OnSOMPaneMapChanged(wxCommandEvent&)
{
    auto& gfx = Graphics::Get(*this);
    auto& objlist = ObjectList::Get(*this);

    world.theMap->SetTexture(Bind::TextureManager::Resolve(gfx, m_imageFile.c_str(), 0));
    world.theMap->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
    objlist.Add(ObjectType_Map, world.theMap);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    m_model = std::make_shared<SurfaceVoxels>(data);
    m_model->SetViewFlags(ObjectViewFlag_Solid);
    m_model->SetTexture(Bind::TextureManager::Resolve(Graphics::Get(*this), m_imageFile.c_str(), 0));

    log_info("%lu voxels will be rendered.", m_model->Voxels().size());

    world.theDataset = std::make_shared<Dataset<3>>(m_model->GenPositions());

    auto& objlist = ObjectList::Get(*this);
    objlist.Add(ObjectType_Model, m_model);
    objlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::OnMenuAddModel(wxCommandEvent& event)
{
    if (world.theDataset == nullptr) {
        wxMessageDialog dialog(&ProjectWindow::Get(*this), "Please import a model from the File menu first.", "Error",
                               wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    // FIXME Better way to get the bounding box
    BoundingBox const bbox = world.theDataset->GetBoundingBox();

    if (event.GetId() == EVT_ADD_UV_SPHERE) {
        auto const& [max, min] = bbox;
        float const radius = glm::length((max - min) * 0.5f);
        glm::vec3 const center = (max + min) * 0.5f;

        auto sphere = std::make_shared<Sphere>(60, 60);

        TransformStack ts;
        ts.PushTranslate(center);
        ts.PushScale(radius, radius, radius);
        sphere->SetTransform(ts);

        auto& objlist = ObjectList::Get(*this);
        objlist.Add(ObjectType_Sphere, sphere);
        objlist.Submit(Renderer::Get(*this));
    }
}

void InitRenderingTechniques(Graphics& gfx)
{
    {
        auto tech = TechniqueManager::Resolve("solid");
        BindStep step;
        std::shared_ptr<UniformBlock> transform, light, mat, view;
        transform = std::make_shared<UniformBlock>();
        light = std::make_shared<UniformBlock>();
        mat = std::make_shared<UniformBlock>();
        view = std::make_shared<UniformBlock>();

        transform->AddElement(UniformBlock::Type::mat4, "model");
        transform->AddElement(UniformBlock::Type::mat4, "viewProj");
        light->AddElement(UniformBlock::Type::vec3f32, "position");
        light->AddElement(UniformBlock::Type::vec3f32, "ambient");
        light->AddElement(UniformBlock::Type::vec3f32, "diffusion");
        light->AddElement(UniformBlock::Type::vec3f32, "specular");
        mat->AddElement(UniformBlock::Type::vec3f32, "ambient");
        mat->AddElement(UniformBlock::Type::vec3f32, "diffusion");
        mat->AddElement(UniformBlock::Type::vec3f32, "specular");
        mat->AddElement(UniformBlock::Type::f32, "shininess");
        view->AddElement(UniformBlock::Type::vec3f32, "viewPos");

        transform->FinalizeLayout();
        light->FinalizeLayout();
        mat->FinalizeLayout();
        view->FinalizeLayout();

        transform->Assign("model", glm::mat4(1.0f));
        transform->Assign("viewProj", gfx.GetViewProjectionMatrix());
        light->Assign("position", gfx.GetCameraPosition());
        light->Assign("ambient", glm::vec3(0.8f, 0.8f, 0.8f));
        light->Assign("diffusion", glm::vec3(0.8f, 0.8f, 0.8f));
        light->Assign("specular", glm::vec3(0.8f, 0.8f, 0.8f));
        mat->Assign("ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        mat->Assign("diffusion", glm::vec3(0.6f, 0.6f, 0.6f));
        mat->Assign("specular", glm::vec3(0.3f, 0.3f, 0.3f));
        mat->Assign("shininess", 256.0f);
        view->Assign("viewPos", gfx.GetCameraPosition());

        transform->SetBIndex(0);
        light->SetBIndex(1);
        mat->SetBIndex(2);
        view->SetBIndex(3);

        tech->AddUniformBlock("transform", transform);
        tech->AddUniformBlock("light", light);
        tech->AddUniformBlock("material", mat);
        tech->AddUniformBlock("view", view);

        std::vector<GLWRInputElementDesc> inputs = {
            { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
            { "normal", GLWRFormat_Float3, 0, 3 * sizeof(float), GLWRInputClassification_PerVertex, 0 },
        };

        auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/SolidDrawable.vert");
        step.AddBindable(vs);
        step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/SolidDrawable.frag"));
        step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *light, "light"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *mat, "material"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *view, "view"));
        step.AddBindable(
            std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));
        tech->AddBindStep(step);
    }
    {
        auto tech = TechniqueManager::Resolve("textured");
        BindStep step;
        std::shared_ptr<UniformBlock> transform, light, mat, view;
        transform = std::make_shared<UniformBlock>();
        light = std::make_shared<UniformBlock>();
        mat = std::make_shared<UniformBlock>();
        view = std::make_shared<UniformBlock>();

        transform->AddElement(UniformBlock::Type::mat4, "model");
        transform->AddElement(UniformBlock::Type::mat4, "viewProj");
        light->AddElement(UniformBlock::Type::vec3f32, "position");
        light->AddElement(UniformBlock::Type::vec3f32, "ambient");
        light->AddElement(UniformBlock::Type::vec3f32, "diffusion");
        light->AddElement(UniformBlock::Type::vec3f32, "specular");
        mat->AddElement(UniformBlock::Type::vec3f32, "ambient");
        mat->AddElement(UniformBlock::Type::vec3f32, "diffusion");
        mat->AddElement(UniformBlock::Type::vec3f32, "specular");
        mat->AddElement(UniformBlock::Type::f32, "shininess");
        view->AddElement(UniformBlock::Type::vec3f32, "viewPos");

        transform->FinalizeLayout();
        light->FinalizeLayout();
        mat->FinalizeLayout();
        view->FinalizeLayout();

        transform->Assign("model", glm::mat4(1.0f));
        transform->Assign("viewProj", gfx.GetViewProjectionMatrix());
        light->Assign("position", gfx.GetCameraPosition());
        light->Assign("ambient", glm::vec3(0.8f, 0.8f, 0.8f));
        light->Assign("diffusion", glm::vec3(0.8f, 0.8f, 0.8f));
        light->Assign("specular", glm::vec3(0.8f, 0.8f, 0.8f));
        mat->Assign("ambient", glm::vec3(0.3f, 0.3f, 0.3f));
        mat->Assign("diffusion", glm::vec3(0.6f, 0.6f, 0.6f));
        mat->Assign("specular", glm::vec3(0.3f, 0.3f, 0.3f));
        mat->Assign("shininess", 256.0f);
        view->Assign("viewPos", gfx.GetCameraPosition());

        transform->SetBIndex(0);
        light->SetBIndex(1);
        mat->SetBIndex(2);
        view->SetBIndex(3);

        tech->AddUniformBlock("transform", transform);
        tech->AddUniformBlock("light", light);
        tech->AddUniformBlock("material", mat);
        tech->AddUniformBlock("view", view);

        GLWRSamplerDesc samplerDesc;
        samplerDesc.coordinateS = GLWRTextureCoordinatesMode_Wrap;
        samplerDesc.coordinateT = GLWRTextureCoordinatesMode_Wrap;
        samplerDesc.coordinateR = GLWRTextureCoordinatesMode_Wrap;
        samplerDesc.filter = GLWRFilter_MinMagNearest_MipNearest;
        std::vector<GLWRInputElementDesc> inputs = {
            { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
            { "normal", GLWRFormat_Float3, 0, 3 * sizeof(float), GLWRInputClassification_PerVertex, 0 },
            { "textureCoord", GLWRFormat_Float2, 0, 6 * sizeof(float), GLWRInputClassification_PerVertex, 0 },
        };

        auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/SolidDrawable.vert");
        step.AddBindable(vs);
        step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/SolidDrawable.frag"));
        step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *light, "light"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *mat, "material"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *view, "view"));
        // step.AddBindable(texture);
        // step.AddBindable(std::make_shared<Bind::Sampler>(gfx, samplerDesc, 0));
        step.AddBindable(
            std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_None }));
        tech->AddBindStep(step);
    }
    {
        std::shared_ptr<UniformBlock> transform, color;
        transform = std::make_shared<UniformBlock>();
        color = std::make_shared<UniformBlock>();
        transform->AddElement(UniformBlock::Type::mat4, "model");
        transform->AddElement(UniformBlock::Type::mat4, "viewProj");
        transform->SetBIndex(0);
        transform->FinalizeLayout();
        color->AddElement(UniformBlock::Type::vec3f32, "color");
        color->FinalizeLayout();
        color->SetBIndex(1);

        transform->Assign("model", glm::mat4(1.0f));
        transform->Assign("viewProj", gfx.GetViewProjectionMatrix());

        std::vector<GLWRInputElementDesc> inputs = {
            { "position", GLWRFormat_Float3, 0, 0, GLWRInputClassification_PerVertex, 0 },
        };

        BindStep step;
        auto vs = std::make_shared<Bind::VertexShaderProgram>(gfx, "shader/WireDrawable.vert");
        step.AddBindable(vs);
        step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
        step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
        step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *color, "color"));
        step.AddBindable(
            std::make_shared<Bind::RasterizerState>(gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));

        {
            auto wire = TechniqueManager::Resolve("wire");
            color->Assign("color", glm::vec3(0.7f, 0.7f, 0.7f));
            wire->AddUniformBlock("transform", transform);
            wire->AddUniformBlock("color", color);
            step = BindStep();
            step.AddBindable(vs);
            step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
            step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *color, "color"));
            step.AddBindable(std::make_shared<Bind::RasterizerState>(
                gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
            wire->AddBindStep(step);
        }
        {
            auto guides = TechniqueManager::Resolve("guides");
            color->Assign("color", glm::vec3(0.3f, 0.3f, 0.3f));
            guides->AddUniformBlock("transform", transform);
            guides->AddUniformBlock("color", color);
            step = BindStep();
            step.AddBindable(vs);
            step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
            step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *color, "color"));
            step.AddBindable(std::make_shared<Bind::RasterizerState>(
                gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
            guides->AddBindStep(step);
        }
        {
            auto xaxis = TechniqueManager::Resolve("x-axis");
            color->Assign("color", glm::vec3(0.8f, 0.3f, 0.3f));
            xaxis->AddUniformBlock("transform", transform);
            xaxis->AddUniformBlock("color", color);
            step = BindStep();
            step.AddBindable(vs);
            step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
            step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *color, "color"));
            step.AddBindable(std::make_shared<Bind::RasterizerState>(
                gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
            xaxis->AddBindStep(step);
        }
        {
            auto yaxis = TechniqueManager::Resolve("y-axis");
            color->Assign("color", glm::vec3(0.3f, 0.8f, 0.3f));
            yaxis->AddUniformBlock("transform", transform);
            yaxis->AddUniformBlock("color", color);

            step = BindStep();
            step.AddBindable(vs);
            step.AddBindable(std::make_shared<Bind::FragmentShaderProgram>(gfx, "shader/WireDrawable.frag"));
            step.AddBindable(std::make_shared<Bind::InputLayout>(gfx, inputs, vs.get()));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *transform, "transform"));
            step.AddBindable(std::make_shared<Bind::UniformBuffer>(gfx, *color, "color"));
            step.AddBindable(std::make_shared<Bind::RasterizerState>(
                gfx, GLWRRasterizerDesc { GLWRFillMode_Solid, GLWRCullMode_Back }));
            yaxis->AddBindStep(step);
       }
    }
}
