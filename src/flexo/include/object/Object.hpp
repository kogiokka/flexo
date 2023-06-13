#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <string>
#include <vector>

#include "EditableMesh.hpp"
#include "Mesh.hpp"
#include "SolidDrawable.hpp"
#include "TexturedDrawable.hpp"
#include "TransformStack.hpp"
#include "WireDrawable.hpp"
#include "Wireframe.hpp"
#include "gfx/DrawableBase.hpp"
#include "gfx/bindable/Texture2D.hpp"
#include "gfx/bindable/TextureManager.hpp"

#define OBJECT_TYPES                                                                                                   \
    X(ObjectType_Plane, "Plane")                                                                                       \
    X(ObjectType_Grid, "Grid")                                                                                         \
    X(ObjectType_Cube, "Cube")                                                                                         \
    X(ObjectType_Guides, "Guides")                                                                                     \
    X(ObjectType_Model, "Model")                                                                                       \
    X(ObjectType_Map, "Map")                                                                                           \
    X(ObjectType_Light, "Light")                                                                                       \
    X(ObjectType_Sphere, "Sphere")                                                                                     \
    X(ObjectType_Torus, "Torus")

#define X(type, name) type,
enum ObjectType : unsigned int { OBJECT_TYPES };
#undef X

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

    using DrawList = std::vector<std::shared_ptr<Drawable>>;

    Object(ObjectType type, EditableMesh mesh = EditableMesh());
    virtual ~Object() = default;

    void SetID(std::string id);
    void SetViewFlags(ObjectViewFlag flags);
    ObjectViewFlag GetViewFlags() const;
    void SetTexture(std::shared_ptr<Bind::Texture2D> texture);
    std::shared_ptr<Bind::Texture2D> GetTexture() const;

    std::string GetID() const;
    ObjectType GetType() const;
    void SetVisible(bool visible);
    bool IsVisible() const;

    virtual void GenerateDrawables(Graphics& gfx);
    virtual DrawList const& GetDrawList();
    virtual std::vector<glm::vec3> GetPositions() const;
    virtual void ApplyTransform();

    void SetLocation(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);

    Transform GetTransform() const;

protected:
    TransformStack GenerateTransformStack();

    ObjectType m_type;
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
