#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "EditableMesh.hpp"
#include "TransformStack.hpp"

TransformStack::TransformStack()
    : stack(glm::mat4(1.0f))
{
}

void TransformStack::PushScale(glm::vec3 scale)
{
    stack = glm::scale(stack, scale);
}

void TransformStack::PushTranslate(glm::vec3 translate)
{
    stack = glm::translate(stack, translate);
}

void TransformStack::PushRotate(float radian, glm::vec3 axis)
{
    stack = glm::rotate(stack, radian, axis);
}

void TransformStack::PushScale(float x, float y, float z)
{
    TransformStack::PushScale(glm::vec3(x, y, z));
}

void TransformStack::PushTranslate(float x, float y, float z)
{
    TransformStack::PushTranslate(glm::vec3(x, y, z));
}

void TransformStack::Apply(EditableMesh& mesh)
{
    for (auto& p : mesh.positions) {
        p = glm::vec3(stack * glm::vec4(p, 1.0f));
    }
}

void TransformStack::Clear()
{
    stack = glm::mat4(1.0f);
}
