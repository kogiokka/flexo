#include "Object.hpp"

#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"
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
    Object::DrawList list;

    // FIXME
    if (!m_texture) {
        m_texture = Bind::TextureManager::Resolve(gfx, "res/images/blank.png", 0);
    }

    switch (m_flags) {
    case ObjectViewFlag_Solid:
        list.push_back(std::make_shared<SolidDrawable>(gfx, GenerateSolidMesh()));
        break;
    case ObjectViewFlag_Textured:
        list.push_back(std::make_shared<TexturedDrawable>(gfx, GenerateTexturedMesh(), m_texture));
        break;
    case ObjectViewFlag_Wire:
        list.push_back(std::make_shared<WireDrawable>(gfx, GenerateWireMesh()));
        break;
    case ObjectViewFlag_SolidWithWireframe: {
        list.push_back(std::make_shared<SolidDrawable>(gfx, GenerateSolidMesh()));
        auto wireframe = std::make_shared<WireDrawable>(gfx, GenerateWireMesh());
        wireframe->SetColor(0.0f, 0.0f, 0.0f);
        list.push_back(wireframe);
    } break;
    case ObjectViewFlag_TexturedWithWireframe: {
        list.push_back(std::make_shared<TexturedDrawable>(gfx, GenerateTexturedMesh(), m_texture));
        auto wireframe = std::make_shared<WireDrawable>(gfx, GenerateWireMesh());
        wireframe->SetColor(0.0f, 0.0f, 0.0f);
        list.push_back(wireframe);
    } break;
    }

    m_drawables = list;

    for (auto& d : m_drawables) {
        d->SetVisible(m_isVisible);
    }
}

Object::DrawList const& Object::GetDrawables() const
{
    return m_drawables;
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

    for (auto& d : m_drawables) {
        d->SetVisible(visible);
    }
}

bool Object::IsVisible() const
{
    return m_isVisible;
}
