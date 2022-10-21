#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

#include <wx/msgdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <rvl.h>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
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
    , m_rvl(nullptr)
{
    Bind(EVT_IMPORT_MODEL, &WatermarkingProject::OnMenuImportModel, this);
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

        drawlist.Add(std::make_shared<VolumetricModel>(Graphics::Get(*this), *m_rvl));
        drawlist.Add(std::make_shared<MapFace>(gfx, world.mapMesh));
        drawlist.Submit(Renderer::Get(*this));
    });
}

WatermarkingProject::~WatermarkingProject()
{
    rvl_destroy(&m_rvl);
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    SelfOrganizingMap::Get(*this).CreateProcedure(world.theMap, world.theDataset);

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
    auto const& neurons = map.mNeurons;
    positions.reserve(neurons.size());
    for (auto const& n : neurons) {
        auto const& w = n.weights;
        positions.emplace_back(w[0], w[1], w[2]);
    }

    int const width = map.mWidth;
    int const height = map.mHeight;
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
            t1 = map.mTexureCoord[x + y * width];
            t2 = map.mTexureCoord[x + 1 + y * width];
            t3 = map.mTexureCoord[x + 1 + (y + 1) * width];
            t4 = map.mTexureCoord[x + (y + 1) * width];

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
    int const width = map.mWidth;
    int const height = map.mHeight;
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
    assert(world.theModel);

    // Update the texture coordinates of the Volumetric Model.
    std::vector<glm::vec2> texcrd;

    int x, y, z;
    float dx, dy, dz;
    RVLByte* data;
    rvl_get_resolution(m_rvl, &x, &y, &z);
    rvl_get_data_buffer(m_rvl, &data);
    rvl_get_voxel_dims(m_rvl, &dx, &dy, &dz);

    const RVLByte model = 255;
    for (int i = 0; i < z; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < x; k++) {
                int index = k + j * x + i * x * y;
                if (data[index] != model)
                    continue;

                glm::vec2 coord;
                float minDist = std::numeric_limits<float>::max();
                glm::vec3 vxPos(k * dx, j * dy, i * dz);

                for (unsigned int n = 0; n < world.theMap->mNeurons.size(); n++) {
                    auto const& node = world.theMap->mNeurons[n];
                    glm::vec3 nodePos(node.weights[0], node.weights[1], node.weights[2]);
                    float const dist = glm::distance(vxPos, nodePos);
                    if (dist < minDist) {
                        minDist = dist;
                        coord = world.theMap->mTexureCoord[n];
                    }
                }
                for (int n = 0; n < world.numVxVerts[index]; n++) {
                    texcrd.emplace_back(coord);
                }
            }
        }
    }

    world.isWatermarked = true;

    // Update the drawlist and the renderer
    auto& drawlist = DrawList::Get(*this);
    for (auto const& d : drawlist) {
        VolumetricModel* ptr = dynamic_cast<VolumetricModel*>(d.get());
        if (ptr != nullptr) {
            for (auto it = ptr->m_binds.begin(); it != ptr->m_binds.end(); it++) {
                Bind::VertexBuffer* vb = dynamic_cast<Bind::VertexBuffer*>(it->get());
                if ((vb != nullptr) && (vb->GetStartAttrib() == 2)) {
                    vb->Update(Graphics::Get(*this), texcrd);
                }
            }
        }
    }
    drawlist.Submit(Renderer::Get(*this));
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

void WatermarkingProject::ImportPolygonalModel(wxString const& path)
{
    if (path.EndsWith(".obj")) {
        OBJImporter objImp;
        world.theModel = std::make_shared<Mesh>(objImp.ReadFile(path.ToStdString()));
    } else if (path.EndsWith(".stl")) {
        STLImporter stlImp;
        world.theModel = std::make_shared<Mesh>(stlImp.ReadFile(path.ToStdString()));
    }

    world.theDataset = std::make_shared<Dataset<3>>(world.theModel->positions);

    SetModelDrawable(std::make_shared<PolygonalModel>(Graphics::Get(*this), *world.theModel));
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    m_rvl = rvl_create_reader(path.c_str());

    int x, y, z;
    float dx, dy, dz;
    RVLByte* data;

    rvl_read_rvl(m_rvl);
    rvl_get_resolution(m_rvl, &x, &y, &z);
    rvl_get_data_buffer(m_rvl, &data);
    rvl_get_voxel_dims(m_rvl, &dx, &dy, &dz);

    if (rvl_get_primitive(m_rvl) != RVLPrimitive_u8) {
        std::cerr << "Wrong RVL format: " << path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    world.theModel = std::make_shared<Mesh>();

    auto& pos = world.theModel->positions;

    const RVLByte model = 255;
    for (int i = 0; i < z; i++) {
        for (int j = 0; j < y; j++) {
            for (int k = 0; k < x; k++) {
                int index = k + j * x + i * x * y;
                if (data[index] == model) {
                    pos.push_back(glm::vec3 { k * dx, j * dy, i * dz });
                }
            }
        }
    }

    Logger::info("%lu voxels will be rendered.", pos.size());

    world.theDataset = std::make_shared<Dataset<3>>(pos);

    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), *m_rvl));
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

    auto& positions = world.theModel->positions;
    auto& texcoord = world.theModel->textureCoords;
    positions.insert(positions.end(), pos.begin(), pos.end());
    texcoord = std::vector<glm::vec2>(positions.size(), glm::vec2(0.0f, 0.0f));

    world.theDataset->Insert(pos);

    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), *m_rvl));
}

void WatermarkingProject::OnMenuAddModel(wxCommandEvent& event)
{
    if (world.theModel == nullptr) {
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
        int const numSegments = 60;
        int const numRings = 60;

        glm::vec3 const center = (max + min) * 0.5f;
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
    }

    world.theModel = std::make_shared<Mesh>(mesh);
    world.theDataset = std::make_shared<Dataset<3>>(mesh.positions);

    SetModelDrawable(std::make_shared<PolygonalModel>(Graphics::Get(*this), mesh));
}

void WatermarkingProject::OnMenuImportModel(wxCommandEvent& event)
{
    wxString const filepath = event.GetString();
    if (filepath.EndsWith(".rvl")) {
        ImportVolumetricModel(filepath);
    } else {
        ImportPolygonalModel(filepath);
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
