#include "object/Object.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/TechniqueManager.hpp"
#include "util/Logger.h"

Object::Object()
    : m_texture(nullptr)
    , m_flags(ObjectViewFlag_Solid)
    , m_isVisible(true)
{
}

Object::~Object()
{
}

void Object::GenerateDrawables(Graphics& gfx)
{
    // FIXME
    if (!m_texture) {
        m_texture = Bind::TextureManager::Resolve(gfx, "res/images/blank.png", 0);
    }

    m_solid = std::make_shared<SolidDrawable>(gfx, GenerateMesh());
    m_textured = std::make_shared<TexturedDrawable>(gfx, GenerateMesh(), m_texture);
    m_wire = std::make_shared<WireDrawable>(gfx, GenerateWireMesh());

    m_solid->AddTechnique(*TechniqueManager::Resolve("solid"));
    m_textured->AddTechnique(*TechniqueManager::Resolve("textured"));
    m_wire->AddTechnique(*TechniqueManager::Resolve("wire"));
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
        TechniqueManager::Resolve("wire")->SetUniform("color", "color", glm::vec3(0.7f, 0.7f, 0.7f));
        list.push_back(m_wire);
        break;
    case ObjectViewFlag_SolidWithWireframe: {
        log_trace("\"%s\" has solid drawable with wireframe", m_id.c_str());
        list.push_back(m_solid);
        TechniqueManager::Resolve("wire")->SetUniform("color", "color", glm::vec3(0.0f, 0.0f, 0.0f));
        list.push_back(m_wire);
    } break;
    case ObjectViewFlag_TexturedWithWireframe: {
        log_trace("\"%s\" has textured drawable with wireframe", m_id.c_str());
        list.push_back(m_textured);
        TechniqueManager::Resolve("wire")->SetUniform("color", "color", glm::vec3(0.0f, 0.0f, 0.0f));
        list.push_back(m_wire);
    } break;
    }

    for (auto& d : list) {
        d->SetVisible(m_isVisible);
    }

    m_drawlist = list;

    return m_drawlist;
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
