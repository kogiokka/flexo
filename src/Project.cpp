#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "SurfaceVoxels.hpp"
#include "VecUtil.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include "gfx/DrawList.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/drawable/LightSource.hpp"
#include "gfx/drawable/MapEdge.hpp"
#include "gfx/drawable/MapFace.hpp"
#include "gfx/drawable/MapVertex.hpp"
#include "gfx/drawable/PolygonalModel.hpp"
#include "gfx/drawable/VolumetricModel.hpp"

WatermarkingProject::WatermarkingProject()
    : m_isMapReady(false)
    , m_frame {}
    , m_panel {}
    , m_model(nullptr)
{
    Bind(EVT_ADD_UV_SPHERE, &WatermarkingProject::OnMenuAddModel, this);
    Bind(EVT_ADD_PLATE_50_BY_50, &WatermarkingProject::OnMenuAddPlate, this);
    Bind(EVT_ADD_PLATE_100_BY_100, &WatermarkingProject::OnMenuAddPlate, this);
    Bind(EVT_ADD_PLATE_200_BY_200, &WatermarkingProject::OnMenuAddPlate, this);

    // FIXME
    Bind(EVT_OPEN_IMAGE, [this](wxCommandEvent& event) {
        world.imagePath = event.GetString().ToStdString();

        auto& gfx = Graphics::Get(*this);
        std::string const filename = event.GetString().ToStdString();
        auto& drawables = DrawList::Get(*this);
        for (auto it = drawables.begin(); it != drawables.end(); it++) {
            {
                MapFace* face = dynamic_cast<MapFace*>(it->get());
                if (face != nullptr) {
                    face->ChangeTexture(gfx, filename.c_str());
                }
            }
            {
                VolumetricModel* model = dynamic_cast<VolumetricModel*>(it->get());
                if (model != nullptr) {
                    model->ChangeTexture(gfx, filename.c_str());
                }
            }
        }

        auto& drawlist = DrawList::Get(*this);
        drawlist.Remove<VolumetricModel>();
        drawlist.Remove<MapFace>();

        SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), m_model->GenMesh()));
        drawlist.Add(std::make_shared<MapFace>(gfx, world.mapMesh));
        drawlist.Submit(Renderer::Get(*this));
    });
}

WatermarkingProject::~WatermarkingProject()
{
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    SelfOrganizingMap::Get(*this).CreateProcedure(world.theMap, world.theDataset);

    auto& drawlist = DrawList::Get(*this);
    auto& gfx = Graphics::Get(*this);
    drawlist.Add(std::make_shared<LightSource>(gfx, world.uvsphere));
    drawlist.Submit(Renderer::Get(*this));

    world.isWatermarked = false;
}

void WatermarkingProject::StopProject()
{
    auto& som = SelfOrganizingMap::Get(*this);
    if (som.IsTraining()) {
        som.ToggleTraining();
    }
    som.StopWorker();
}

void WatermarkingProject::BuildMapMesh() const
{
    if (!m_isMapReady)
        return;

    Map<3, 2> const& map = *world.theMap;
    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = map.nodes;
    positions.reserve(neurons.size());
    for (auto const& n : neurons) {
        auto const& w = n.weights;
        positions.emplace_back(w[0], w[1], w[2]);
    }

    int const width = map.width;
    int const height = map.height;
    // float const divisor = 1.1f; // FIXME

    float const w = static_cast<float>(width - 1);
    float const h = static_cast<float>(height - 1);
    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {

            unsigned int const idx = y * width + x;
            glm::vec3 p1, p2, p3, p4;
            glm::vec3 n1, n2;
            glm::vec2 t1, t2, t3, t4;
            Face f1, f2;

            /**
             *  4-----3
             *  |     |
             *  |     |
             *  1-----2
             */
            p1 = positions[idx]; // [x, y]
            p2 = positions[idx + 1]; // [x + 1, y]
            p3 = positions[idx + width + 1]; // [x + 1, y + 1]
            p4 = positions[idx + width]; // [x, y + 1]

            // Normals
            n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            n2 = glm::normalize(glm::cross(p3 - p1, p4 - p1));

            // TextureCoords
            // t1 = glm::vec2(x / divisor, y / divisor);
            // t2 = glm::vec2((x + 1) / divisor, y / divisor);
            // t3 = glm::vec2((x + 1) / divisor, (y + 1) / divisor);
            // t4 = glm::vec2(x / divisor, (y + 1) / divisor);
            SetVec2(t1, map.nodes[x + y * width].uv);
            SetVec2(t2, map.nodes[x + 1 + y * width].uv);
            SetVec2(t3, map.nodes[x + 1 + (y + 1) * width].uv);
            SetVec2(t4, map.nodes[x + (y + 1) * width].uv);

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p1);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p4);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t2);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t1);
            mesh.textureCoords.push_back(t3);
            mesh.textureCoords.push_back(t4);
        }
    }

    world.neurons.positions = positions;
    world.mapMesh = mesh;

    // FIXME?
    UpdateMapEdges();
}

void WatermarkingProject::UpdateMapEdges() const
{
    std::vector<unsigned int> indices;

    auto const& map = *world.theMap;
    int const width = map.width;
    int const height = map.height;
    for (int i = 0; i < height - 1; ++i) {
        for (int j = 0; j < width - 1; ++j) {
            indices.push_back(i * width + j);
            indices.push_back(i * width + j + 1);
            indices.push_back(i * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j + 1);
            indices.push_back((i + 1) * width + j);
            indices.push_back((i + 1) * width + j);
            indices.push_back(i * width + j);
        }
    }

    world.mapEdges = indices;
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
    m_model->Parameterize(*world.theMap);
    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), m_model->GenMesh()));

    world.isWatermarked = true;
}

void WatermarkingProject::UpdateMapGraphics()
{
    // TODO: Better solution
    m_isMapReady = true;
    BuildMapMesh();

    std::vector<std::shared_ptr<DrawableBase>> drawables;
    auto& gfx = Graphics::Get(*this);
    drawables.push_back(std::make_shared<MapVertex>(gfx, world.uvsphere, world.neurons));
    drawables.push_back(std::make_shared<MapEdge>(gfx, world.neurons, world.mapEdges));
    drawables.push_back(std::make_shared<MapFace>(gfx, world.mapMesh));

    auto& drawlist = DrawList::Get(*this);
    drawlist.Remove<MapVertex>();
    drawlist.Remove<MapEdge>();
    drawlist.Remove<MapFace>();
    for (auto& d : drawables) {
        drawlist.Add(d);
    }
    drawlist.Submit(Renderer::Get(*this));
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    VolumetricModelData data;
    data.Read(path.ToStdString().c_str());

    m_model = std::make_unique<SurfaceVoxels>(data);
    Logger::info("%lu voxels will be rendered.", m_model->Voxels().size());

    world.theDataset = std::make_shared<Dataset<3>>(m_model->GenPositions());
    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), m_model->GenMesh()));
}

void WatermarkingProject::OnMenuAddPlate(wxCommandEvent& event)
{
    if (!world.theDataset) {
        wxMessageDialog dialog(&ProjectWindow::Get(*this), "Please import a model from the File menu first.", "Error",
                               wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    int width = event.GetInt();
    int height = event.GetInt();

    auto box = world.theDataset->GetBoundingBox();
    float xDiff = (box.max.x - box.min.x) / static_cast<float>(width);
    float yDiff = (box.max.y - box.min.y) / static_cast<float>(height);

    std::vector<glm::vec3> pos;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            pos.emplace_back(box.min.x + i * xDiff, box.min.y + j * yDiff, box.min.z);
        }
    }
    auto mesh = m_model->GenMesh();
    mesh.positions.insert(mesh.positions.end(), pos.begin(), pos.end());
    mesh.textureCoords = std::vector<glm::vec2>(mesh.positions.size(), glm::vec2(0.0f, 0.0f));

    world.theDataset->Insert(pos);

    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), mesh));
}

void WatermarkingProject::OnMenuAddModel(wxCommandEvent& event)
{
    if (world.theDataset == nullptr) {
        wxMessageDialog dialog(&ProjectWindow::Get(*this), "Please import a model from the File menu first.", "Error",
                               wxCENTER | wxICON_ERROR);
        dialog.ShowModal();
        return;
    }

    Mesh mesh;

    // FIXME Better way to get the bounding box
    BoundingBox const bbox = world.theDataset->GetBoundingBox();

    if (event.GetId() == EVT_ADD_UV_SPHERE) {
        auto const& [max, min] = bbox;
        float const radius = glm::length((max - min) * 0.5f);
        glm::vec3 const center = (max + min) * 0.5f;

        Mesh const mesh = CreateUVSphereModel(radius, 60, 60, center);
        world.theDataset = std::make_shared<Dataset<3>>(mesh.positions);

        SetModelDrawable(std::make_shared<PolygonalModel>(Graphics::Get(*this), mesh));
    }
}

void WatermarkingProject::SetModelDrawable(std::shared_ptr<DrawableBase> drawable)
{
    auto& drawlist = DrawList::Get(*this);
    drawlist.Remove<VolumetricModel>();
    drawlist.Remove<PolygonalModel>();
    drawlist.Add(drawable);
    drawlist.Submit(Renderer::Get(*this));
}

Mesh WatermarkingProject::CreateUVSphereModel(float radius, int numSegments, int numRings, glm::vec3 center)
{
    Mesh mesh;

    float deltaLong = glm::radians(360.0f) / numSegments;
    float deltaLat = glm::radians(180.0f) / numRings;

    std::vector<glm::vec3> temp;
    temp.resize((numRings - 1) * numSegments + 2);
    temp.front() = center + radius * glm::vec3(0.0f, cos(0.0f), 0.0f);
    temp.back() = center + radius * glm::vec3(0.0f, cos(glm::radians(180.0f)), 0.0f);

    // Iterate through the rings without the first and the last.
    for (int i = 1; i < numRings; i++) {
        for (int j = 0; j < numSegments; j++) {
            float const theta = deltaLat * i;
            float const phi = glm::radians(180.0f) - deltaLong * j;
            glm::vec3 const coord = radius * glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
            int const idx = (i - 1) * numSegments + j + 1;
            temp[idx] = center + coord;
        }
    }

    for (int j = 0; j < numSegments; j++) {
        auto const& p1 = temp[0];
        auto const& p2 = temp[(j + 1) % numSegments + 1];
        auto const& p3 = temp[j % numSegments + 1];
        glm::vec3 const normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

        mesh.positions.push_back(p1);
        mesh.positions.push_back(p2);
        mesh.positions.push_back(p3);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);
    }

    for (int i = 0; i < numRings - 2; i++) {
        for (int j = 0; j < numSegments; j++) {
            /**
             *  4---3
             *  |   |
             *  1---2
             */
            int k1 = (i * numSegments + (j % numSegments)) + 1;
            int k2 = (i * numSegments + ((j + 1) % numSegments)) + 1;
            int k3 = ((i + 1) * numSegments + ((j + 1) % numSegments)) + 1;
            int k4 = ((i + 1) * numSegments + (j % numSegments)) + 1;
            glm::vec3 p1, p2, p3, p4;
            glm::vec3 n1, n2;
            p1 = temp[k1]; // [x, y]
            p2 = temp[k2]; // [x + 1, y]
            p3 = temp[k3]; // [x + 1, y + 1]
            p4 = temp[k4]; // [x, y + 1]
            n1 = glm::normalize(glm::cross(p2 - p1, p3 - p1));
            n2 = glm::normalize(glm::cross(p3 - p1, p4 - p1));

            mesh.positions.push_back(p1);
            mesh.positions.push_back(p2);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p1);
            mesh.positions.push_back(p3);
            mesh.positions.push_back(p4);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n1);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
            mesh.normals.push_back(n2);
        }
    }
    for (int j = 0; j < numSegments; j++) {
        auto const& p1 = temp[((numRings - 2) * numSegments + 1) + (j % numSegments)];
        auto const& p2 = temp[((numRings - 2) * numSegments + 1) + ((j + 1) % numSegments)];
        auto const& p3 = temp.back();
        glm::vec3 const normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

        mesh.positions.push_back(p1);
        mesh.positions.push_back(p2);
        mesh.positions.push_back(p3);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);
        mesh.normals.push_back(normal);
    }

    return mesh;
}
