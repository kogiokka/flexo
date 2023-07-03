#include <algorithm>
#include <array>
#include <cstdio>
#include <string>

#include "EditableMesh.hpp"
#include "Project.hpp"
#include "RandomRealNumber.hpp"
#include "Scene.hpp"
#include "object/SurfaceVoxels.hpp"
#include "pane/OutlinerPane.hpp"
#include "pane/SceneViewportPane.hpp"
#include "pane/SelfOrganizingMapPane.hpp"

constexpr auto PI = 3.14159265358979323846;

#define X(type, name) name,
static std::string ObjectTypeNames[] = { OBJECT_TYPES };
#undef X

// Register factory: ObjectList
static FlexoProject::AttachedObjects::RegisteredFactory const factoryKey {
    [](FlexoProject& project) -> SharedPtr<Scene> { return std::make_shared<Scene>(project); }
};

Scene& Scene::Get(FlexoProject& project)
{
    return project.AttachedObjects::Get<Scene>(factoryKey);
}

Scene const& Scene::Get(FlexoProject const& project)
{
    return Get(const_cast<FlexoProject&>(project));
}

Scene::Scene(FlexoProject& project)
    : m_list()
    , m_project(project)
{
}

void Scene::AddPlane(float size)
{
    auto obj = std::make_shared<Object>(ObjectType_Plane, ConstructPlane(size));
    AcceptObject(obj);
}

void Scene::AddGrid(int numXDiv, int numYDiv, float size)
{
    auto obj = std::make_shared<Object>(ObjectType_Grid, ConstructGrid(numXDiv, numYDiv, size));
    AcceptObject(obj);
}

void Scene::AddCube(float size)
{
    auto obj = std::make_shared<Object>(ObjectType_Cube, ConstructCube(size));
    AcceptObject(obj);
}

void Scene::AddUVSphere(int numSegments, int numRings, float radius)
{
    auto obj = std::make_shared<Object>(ObjectType_Sphere, ConstructUVSphere(numSegments, numRings, radius));
    AcceptObject(obj);
}

void Scene::AddTorus(int majorSeg, int minorSeg, float majorRad, float minorRad)
{
    auto obj = std::make_shared<Object>(ObjectType_Torus, ConstructTorus(majorSeg, minorSeg, majorRad, minorRad));
    AcceptObject(obj);
}

void Scene::AddMap(int width, int height, MapFlags flags, MapInitState initState)
{
    auto map = std::make_shared<Map<3, 2>>();

    float const w = static_cast<float>(width - 1);
    float const h = static_cast<float>(height - 1);

    switch (initState) {
    default:
    case MapInitState_Random: {
        BoundingBox box = { { 5.0f, 5.0f, 5.0f }, { -5.0f, -5.0f, -5.0f } };
        RandomRealNumber<float> xRng(box.min.x, box.max.x);
        RandomRealNumber<float> yRng(box.min.y, box.max.y);
        RandomRealNumber<float> zRng(box.min.z, box.max.z);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                map->nodes.emplace_back(Vec3f { xRng.scalar(), yRng.scalar(), zRng.scalar() },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    } break;
    case MapInitState_Plane: {
        float dx = 2.0f / static_cast<float>(width - 1);
        float dy = 2.0f / static_cast<float>(height - 1);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                map->nodes.emplace_back(Vec3f { -1.0f + i * dx, -1.0f + j * dy, 0.005f },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    } break;
    case MapInitState_Cylinder: {
        float dr = glm::radians(360.0f) / static_cast<float>(width - 1);
        float dz = 2.0f * PI * 1.0f / static_cast<float>(height - 1);

        for (int j = 0; j < height; ++j) {
            for (int i = 0; i < width; ++i) {
                auto rad = i * dr;
                map->nodes.emplace_back(Vec3f { cos(rad), sin(rad), j * dz },
                                        Vec2f { static_cast<float>(i), static_cast<float>(j) }, Vec2f { i / w, j / h });
            }
        }
    } break;
    }

    auto& gfx = SceneViewportPane::Get(m_project).GetGL();

    map->size.x = width;
    map->size.y = height;
    map->flags = flags;
    map->GenerateDrawables(gfx);
    map->SetTexture(Bind::TextureManager::Resolve(gfx, "images/blank.png", 0));
    map->SetViewFlags(ObjectViewFlag_TexturedWithWireframe);
    AcceptObject(map);
}

void Scene::AddModel(VolumetricModelData const& data)
{
    auto obj = std::make_shared<SurfaceVoxels>(data);
    AcceptObject(obj);
    log_info("%lu voxels will be rendered.", obj->Voxels().size());
}

void Scene::Delete(std::string const& id)
{
    for (auto it = m_list.begin(); it != m_list.end();) {
        if ((*it)->GetID() == id) {
            m_list.erase(it);
            break;
        } else {
            it++;
        }
    }
}

Scene::iterator Scene::begin()
{
    return m_list.begin();
}

Scene::iterator Scene::end()
{
    return m_list.end();
}

Scene::const_iterator Scene::cbegin() const
{
    return m_list.cbegin();
}

Scene::const_iterator Scene::cend() const
{
    return m_list.cend();
}

Scene::size_type Scene::size() const
{
    return m_list.size();
}

void Scene::AcceptObject(std::shared_ptr<Object> object)
{
    auto type = object->GetType();
    auto it = m_typeCount.find(type);

    std::string id;
    if (it == m_typeCount.end()) {
        m_typeCount.emplace(type, 0);
        id = ObjectTypeNames[type];
    } else {
        it->second += 1;
        char buf[4];
        snprintf(buf, 4, "%03u", it->second);
        id = ObjectTypeNames[type] + "." + std::string(buf);
    }

    object->SetID(id);
    object->GenerateDrawables(SceneViewportPane::Get(m_project).GetGL());
    m_list.push_back(object);

    wxCommandEvent event(EVT_OUTLINER_ADD_OBJECT);
    event.SetClientData(new OutlinerItemData(object));
    m_project.ProcessEvent(event);
}
