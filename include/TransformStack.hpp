#ifndef TRNASFORM_STACK_H
#define TRNASFORM_STACK_H

#include <glm/glm.hpp>

struct EditableMesh;

struct TransformStack {
    TransformStack();
    void PushScale(glm::vec3 scale);
    void PushTranslate(glm::vec3 translate);
    void PushRotate(float radian, glm::vec3 axis);
    void PushScale(float x, float y, float z);
    void PushTranslate(float x, float y, float z);
    void Clear();
    void Apply(EditableMesh& mesh);

private:
    glm::mat4 stack;
};

#endif
