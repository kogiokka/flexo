#include "object/Object.hpp"
#include "gfx/Graphics.hpp"
#include "log/Logger.h"
#include "Colors.hpp"

Object::Object(ObjectType type, EditableMesh mesh)
    : m_type(type)
    , m_texture(nullptr)
    , m_flags(ObjectViewFlag_Solid)
    , m_isVisible(true)
    , m_mesh(mesh)
{
}

void Object::GenerateDrawables(Graphics& gfx)
{
    // FIXME
    if (!m_texture) {
        m_texture = Bind::TextureManager::Resolve(gfx, "images/blank.png", 0);
    }
    auto m = m_mesh.GenerateMesh();
    m_solid = std::make_shared<SolidDrawable>(gfx, m);
    m_textured = std::make_shared<TexturedDrawable>(gfx, m_mesh.GenerateMesh(), m_texture);
    m_wire = std::make_shared<WireDrawable>(gfx, m_mesh.GenerateWireframe());
}

std::vector<glm::vec3> Object::GetPositions() const
{
    return m_mesh.positions;
}

Object::DrawList const& Object::GetDrawList()
{
    Object::DrawList list;

    switch (m_flags) {
    case ObjectViewFlag_Solid:
        log_trace("\"%s\" has solid drawable", m_id.c_str());
        list.push_back(m_solid);
        break;
    case ObjectViewFlag_Textured:
        log_trace("\"%s\" has textured drawable", m_id.c_str());
        list.push_back(m_textured);
        break;
    case ObjectViewFlag_Wire:
        log_trace("\"%s\" has wire drawable", m_id.c_str());
        m_wire->SetColor(LIGHT_GREY);
        list.push_back(m_wire);
        break;
    case ObjectViewFlag_SolidWithWireframe: {
        log_trace("\"%s\" has solid drawable with wireframe", m_id.c_str());
        list.push_back(m_solid);
        m_wire->SetColor(BLACK);
        list.push_back(m_wire);
    } break;
    case ObjectViewFlag_TexturedWithWireframe: {
        log_trace("\"%s\" has textured drawable with wireframe", m_id.c_str());
        list.push_back(m_textured);
        m_wire->SetColor(BLACK);
        list.push_back(m_wire);
    } break;
    }

    auto modelMat = GenerateTransformStack().GenerateMatrix();

    m_drawlist.clear();
    for (auto& d : list) {
        if (d) {
            d->SetVisible(m_isVisible);
            d->SetTransform(modelMat);
            m_drawlist.push_back(d);
        }
    }

    return m_drawlist;
}

void Object::ApplyTransform()
{
    auto st = GenerateTransformStack();
    st.Apply(m_mesh.positions);
    m_transform = Transform();
}

void Object::SetViewFlags(ObjectViewFlag flags)
{
    m_flags = flags;
}

ObjectViewFlag Object::GetViewFlags() const
{
    return m_flags;
}

void Object::SetID(std::string id)
{
    m_id = id;
}

void Object::SetTexture(std::shared_ptr<Bind::Texture2D> texture)
{
    m_texture = texture;
}

std::shared_ptr<Bind::Texture2D> Object::GetTexture() const
{
    return m_texture;
}

ObjectType Object::GetType() const
{
    return m_type;
}

std::string Object::GetID() const
{
    return m_id;
}

void Object::SetVisible(bool visible)
{
    m_isVisible = visible;

    for (auto& d : m_drawlist) {
        d->SetVisible(visible);
    }
}

bool Object::IsVisible() const
{
    return m_isVisible;
}

void Object::SetLocation(float x, float y, float z)
{
    m_transform.location = glm::vec3(x, y, z);

    TransformStack st = GenerateTransformStack();
    for (auto& d : m_drawlist) {
        d->SetTransform(st.GenerateMatrix());
    }
}

void Object::SetRotation(float x, float y, float z)
{
    m_transform.rotation = glm::vec3(x, y, z);

    TransformStack st = GenerateTransformStack();
    for (auto& d : m_drawlist) {
        d->SetTransform(st.GenerateMatrix());
    }
}

void Object::SetScale(float x, float y, float z)
{
    m_transform.scale = glm::vec3(x, y, z);

    TransformStack st = GenerateTransformStack();
    for (auto& d : m_drawlist) {
        d->SetTransform(st.GenerateMatrix());
    }
}

Object::Transform Object::GetTransform() const
{
    return m_transform;
}

TransformStack Object::GenerateTransformStack()
{
    using namespace glm;
    TransformStack st;
    st.PushTranslate(m_transform.location);
    st.PushRotate(radians(m_transform.rotation.x), vec3(1.0f, 0.0f, 0.0f));
    st.PushRotate(radians(m_transform.rotation.y), vec3(0.0f, 1.0f, 0.0f));
    st.PushRotate(radians(m_transform.rotation.z), vec3(0.0f, 0.0f, 1.0f));
    st.PushScale(m_transform.scale);
    return st;
}
