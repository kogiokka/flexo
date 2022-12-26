#ifndef TRNASFORM_STACK_H
#define TRNASFORM_STACK_H

#include <memory>
#include <vector>

#include <glm/glm.hpp>

struct TransformStack {
    struct Cmd {
        virtual ~Cmd() = default;
        virtual void Push(glm::mat4& mat) = 0;
    };

    struct TranslateCmd;
    struct RotateCmd;
    struct ScaleCmd;
    TransformStack();
    void PushScale(glm::vec3 scale);
    void PushTranslate(glm::vec3 translate);
    void PushRotate(float radian, glm::vec3 axis);
    void PushScale(float x, float y, float z);
    void PushTranslate(float x, float y, float z);
    void Pop();
    void Clear();
    void Apply(std::vector<glm::vec3>& positions);
    glm::mat4 GenerateMatrix();

private:
    std::vector<std::unique_ptr<Cmd>> m_cmdStack;
};

#endif

