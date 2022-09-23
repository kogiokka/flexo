#include <algorithm>
#include <cmath>

#include <wx/msgdlg.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Project.hpp"
#include "ProjectWindow.hpp"
#include "SelfOrganizingMap.hpp"
#include "World.hpp"
#include "assetlib/OBJ/OBJImporter.hpp"
#include "assetlib/STL/STLImporter.hpp"
#include "common/Logger.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/Renderer.hpp"
#include "gfx/drawable/LatticeEdge.hpp"
#include "gfx/drawable/LatticeFace.hpp"
#include "gfx/drawable/LatticeVertex.hpp"
#include "gfx/drawable/LightSource.hpp"
#include "gfx/drawable/PolygonalModel.hpp"
#include "gfx/drawable/VolumetricModel.hpp"

WatermarkingProject::WatermarkingProject()
    : m_isLatticeReady(false)
    , m_frame {}
    , m_panel {}
{
    Bind(EVT_IMPORT_MODEL, &WatermarkingProject::OnMenuImportModel, this);
    Bind(EVT_ADD_UV_SPHERE, &WatermarkingProject::OnMenuAddModel, this);
    Bind(EVT_ADD_PLATE_50_BY_50, &WatermarkingProject::OnMenuAddPlate, this);
    Bind(EVT_ADD_PLATE_100_BY_100, &WatermarkingProject::OnMenuAddPlate, this);
    Bind(EVT_ADD_PLATE_200_BY_200, &WatermarkingProject::OnMenuAddPlate, this);
}

void WatermarkingProject::CreateProject()
{
    assert(world.theDataset);

    SelfOrganizingMap::Get(*this).CreateProcedure(*LatticeList::Get(*this).GetCurrent(), world.theDataset);

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

void WatermarkingProject::BuildLatticeMesh() const
{
    if (!m_isLatticeReady)
        return;

    Lattice<3, 2> const& lattice = *LatticeList::Get(*this).GetCurrent();
    Mesh mesh;

    std::vector<glm::vec3> positions;

    // Positions
    auto const& neurons = lattice.mNeurons;
    positions.reserve(neurons.size());
    for (auto const& n : neurons) {
        auto const& w = n.mWeights;
        positions.emplace_back(w[0], w[1], w[2]);
    }

    int const width = lattice.mWidth;
    int const height = lattice.mHeight;
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
            t1 = glm::vec2(x / w, y / h);
            t2 = glm::vec2((x + 1) / w, y / h);
            t3 = glm::vec2((x + 1) / w, (y + 1) / h);
            t4 = glm::vec2(x / w, (y + 1) / h);

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
    world.latticeMesh = mesh;

    // FIXME?
    UpdateLatticeEdges();
}

void WatermarkingProject::UpdateLatticeEdges() const
{
    std::vector<unsigned int> indices;

    auto lattice = LatticeList::Get(*this).GetCurrent();
    int const width = lattice->mWidth;
    int const height = lattice->mHeight;
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

    world.latticeEdges = indices;
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
    std::vector<glm::vec2> textureCoords;
    textureCoords.reserve(world.theModel->textureCoords.size());

    for (glm::vec3 const& vp : world.theModel->positions) {
        glm::vec2 coord = world.latticeMesh.textureCoords.front();
        float minDist = glm::distance(vp, world.latticeMesh.positions.front());
        // TODO: Deal with the duplicate calculations
        for (unsigned int i = 1; i < world.latticeMesh.positions.size(); i++) {
            auto dist = glm::distance(vp, world.latticeMesh.positions[i]);
            if (dist < minDist) {
                minDist = dist;
                coord = world.latticeMesh.textureCoords[i];
            }
        }
        textureCoords.push_back(coord);
    }
    world.theModel->textureCoords = textureCoords;
    world.isWatermarked = true;
}

void WatermarkingProject::UpdateLatticeGraphics()
{
    // TODO: Better solution
    m_isLatticeReady = true;
    BuildLatticeMesh();

    std::vector<std::shared_ptr<DrawableBase>> drawables;
    auto& gfx = Graphics::Get(*this);
    drawables.push_back(std::make_shared<LatticeVertex>(gfx, world.uvsphere, world.neurons));
    drawables.push_back(std::make_shared<LatticeEdge>(gfx, world.neurons, world.latticeEdges));
    drawables.push_back(std::make_shared<LatticeFace>(gfx, world.latticeMesh));
    SetLatticeDrawables(drawables);
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

    world.theDataset = std::make_shared<InputData>(world.theModel->positions);
    SetModelDrawable(std::make_shared<PolygonalModel>(Graphics::Get(*this), *world.theModel));

    auto& renderer = Renderer::Get(*this);
    renderer.SetCameraView(world.theDataset->GetBoundingBox());
}

void WatermarkingProject::ImportVolumetricModel(wxString const& path)
{
    VolumeData data;

    world.theModel = std::make_shared<Mesh>();
    auto& pos = world.theModel->positions;
    auto& texcoord = world.theModel->textureCoords;

    if (!data.read(path.ToStdString())) {
        Logger::error(R"(Failed to read volumetric model: "%s")", path.ToStdString().c_str());
        return;
    }

    const int model = 252;
    const int isovalue = 0;
    auto reso = data.resolution();
    for (int x = 0; x < reso.x; x++) {
        for (int y = 0; y < reso.y; y++) {
            for (int z = 0; z < reso.z; z++) {
                if (data.value(x, y, z) >= model) {
                    if (x + 1 < reso.x) {
                        if (data.value(x + 1, y, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (x - 1 >= 0) {
                        if (data.value(x - 1, y, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (y + 1 < reso.y) {
                        if (data.value(x, y + 1, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (y - 1 >= 0) {
                        if (data.value(x, y - 1, z) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (z + 1 < reso.z) {
                        if (data.value(x, y, z + 1) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                    if (z - 1 >= 0) {
                        if (data.value(x, y, z - 1) <= isovalue) {
                            pos.push_back(glm::vec3 { x, y, z });
                            continue;
                        }
                    }
                }
            }
        }
    }

    texcoord = std::vector<glm::vec2>(pos.size(), glm::vec2(0.0f, 0.0f));
    Logger::info("%lu voxels will be rendered.", pos.size());

    world.theDataset = std::make_shared<InputData>(pos);
    SetModelDrawable(std::make_shared<VolumetricModel>(Graphics::Get(*this), world.cube, *world.theModel));

    auto& renderer = Renderer::Get(*this);
    renderer.SetCameraView(world.theDataset->GetBoundingBox());
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

    world.theDataset->Insert(pos);
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
    world.theDataset = std::make_shared<InputData>(mesh.positions);
    SetModelDrawable(std::make_shared<PolygonalModel>(Graphics::Get(*this), mesh));

    auto& renderer = Renderer::Get(*this);
    renderer.SetCameraView(world.theDataset->GetBoundingBox());
}

void WatermarkingProject::OnMenuImportModel(wxCommandEvent& event)
{
    wxString const filepath = event.GetString();
    if (filepath.EndsWith(".toml")) {
        ImportVolumetricModel(filepath);
    } else {
        ImportPolygonalModel(filepath);
    }
}

void WatermarkingProject::SetModelDrawable(std::shared_ptr<DrawableBase> drawable)
{
    auto& renderer = Renderer::Get(*this);
    auto& tasks = renderer.GetTasks();
    bool (*const FindTask)(Task&) = [](Task& task) {
        auto* dptr = task.mDrawable;
        return (dynamic_cast<PolygonalModel*>(dptr) || dynamic_cast<VolumetricModel*>(dptr));
    };
    bool (*const FindDrawable)(std::shared_ptr<DrawableBase>&) = [](std::shared_ptr<DrawableBase>& drawable) {
        auto* dptr = drawable.get();
        return (dynamic_cast<PolygonalModel*>(dptr) || dynamic_cast<VolumetricModel*>(dptr));
    };

    tasks.erase(std::remove_if(tasks.begin(), tasks.end(), FindTask), tasks.end());
    m_drawables.erase(std::remove_if(m_drawables.begin(), m_drawables.end(), FindDrawable), m_drawables.end());
    drawable->Submit(renderer);
    m_drawables.push_back(drawable);
}

void WatermarkingProject::SetLatticeDrawables(std::vector<std::shared_ptr<DrawableBase>>& drawables)
{
    auto& renderer = Renderer::Get(*this);
    auto& tasks = renderer.GetTasks();
    bool (*const FindTask)(Task&) = [](Task& task) {
        auto* dptr = task.mDrawable;
        return (dynamic_cast<LatticeVertex*>(dptr) || dynamic_cast<LatticeEdge*>(dptr)
                || dynamic_cast<LatticeFace*>(dptr));
    };
    bool (*const FindDrawable)(std::shared_ptr<DrawableBase>&) = [](std::shared_ptr<DrawableBase>& drawable) {
        auto* dptr = drawable.get();
        return (dynamic_cast<LatticeVertex*>(dptr) || dynamic_cast<LatticeEdge*>(dptr)
                || dynamic_cast<LatticeFace*>(dptr));
    };

    tasks.erase(std::remove_if(tasks.begin(), tasks.end(), FindTask), tasks.end());
    m_drawables.erase(std::remove_if(m_drawables.begin(), m_drawables.end(), FindDrawable), m_drawables.end());
    std::for_each(drawables.begin(), drawables.end(), [&renderer](auto& d) { d->Submit(renderer); });
    m_drawables.insert(m_drawables.end(), drawables.begin(), drawables.end());
}
