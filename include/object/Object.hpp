#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include <vector>

#include "Mesh.hpp"
#include "Wireframe.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"

class Graphics;

typedef int ObjectViewFlag;

enum ObjectViewFlag_ {
    ObjectViewFlag_Solid,
    ObjectViewFlag_Textured,
    ObjectViewFlag_Wire,
    ObjectViewFlag_SolidWithWireframe,
    ObjectViewFlag_TexturedWithWireframe,
};

class Object
{
public:
    using DrawList = std::vector<std::shared_ptr<DrawableBase>>;

    Object();
    ~Object();
    virtual void GenerateDrawables(Graphics& gfx);
    virtual DrawList const& GetDrawList();

    void SetID(std::string id);
    void SetViewFlags(ObjectViewFlag flags);
    ObjectViewFlag GetViewFlags() const;
    void SetTexture(std::shared_ptr<Bind::Texture2D> texture);

    std::string GetID() const;
    void SetVisible(bool visible);
    bool IsVisible() const;

    virtual Mesh GenerateMesh() const = 0;
    virtual Wireframe GenerateWireMesh() const = 0;

protected:
    std::string m_id;
    std::shared_ptr<Bind::Texture2D> m_texture;
    ObjectViewFlag m_flags;
    bool m_isVisible;
    std::shared_ptr<SolidDrawable> m_solid;
    std::shared_ptr<TexturedDrawable> m_textured;
    std::shared_ptr<WireDrawable> m_wire;
    DrawList m_drawlist;
};

#endif
