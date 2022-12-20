#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include <vector>

#include "Mesh.hpp"
#include "Wireframe.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/DrawableBase.hpp"

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
    DrawList const& GetDrawables() const;

    void SetID(std::string id);
    void SetViewFlags(ObjectViewFlag flags);
    ObjectViewFlag GetViewFlags() const;
    void SetTexture(std::shared_ptr<Bind::Texture2D> texture);

    std::string GetID() const;
    void SetVisible(bool visible);
    bool IsVisible() const;

protected:
    virtual Mesh GenerateSolidMesh() const = 0;
    virtual Mesh GenerateTexturedMesh() const = 0;
    virtual Wireframe GenerateWireMesh() const = 0;

    std::string m_id;
    std::shared_ptr<Bind::Texture2D> m_texture;
    DrawList m_drawables;
    ObjectViewFlag m_flags;
    bool m_isVisible;
};

#endif

