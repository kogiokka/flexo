#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include <vector>

#include "EditableMesh.hpp"
#include "Mesh.hpp"
#include "TransformStack.hpp"
#include "Wireframe.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TextureManager.hpp"
#include "gfx/drawable/DrawableBase.hpp"
#include "gfx/drawable/SolidDrawable.hpp"
#include "gfx/drawable/TexturedDrawable.hpp"
#include "gfx/drawable/WireDrawable.hpp"

class Graphics;
struct TransformStack;

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
    struct Transform {
        Transform()
        {
            location = glm::vec3(0.0f, 0.0f, 0.0f);
            rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            scale = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        glm::vec3 location;
        glm::vec3 rotation;
        glm::vec3 scale;
    };

    using DrawList = std::vector<std::shared_ptr<DrawableBase>>;

    Object();
    ~Object();

    void SetID(std::string id);
    void SetViewFlags(ObjectViewFlag flags);
    ObjectViewFlag GetViewFlags() const;
    void SetTexture(std::shared_ptr<Bind::Texture2D> texture);

    std::string GetID() const;
    void SetVisible(bool visible);
    bool IsVisible() const;

    virtual void GenerateDrawables(Graphics& gfx);
    virtual DrawList const& GetDrawList();
    virtual std::vector<glm::vec3> GetPositions() const;
    virtual void ApplyTransform() = 0;

    void SetLocation(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);

    Transform GetTransform() const;

protected:
    TransformStack GenerateTransformStack();

    std::string m_id;
    std::shared_ptr<Bind::Texture2D> m_texture;
    ObjectViewFlag m_flags;
    bool m_isVisible;
    std::shared_ptr<SolidDrawable> m_solid;
    std::shared_ptr<TexturedDrawable> m_textured;
    std::shared_ptr<WireDrawable> m_wire;
    DrawList m_drawlist;

    Transform m_transform;
    EditableMesh m_mesh;
};

#endif