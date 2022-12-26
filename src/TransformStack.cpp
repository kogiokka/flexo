#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TransformStack.hpp"

struct TransformStack::TranslateCmd : public TransformStack::Cmd {
    TranslateCmd(glm::vec3 translate)
        : translate(translate) {};
    virtual ~TranslateCmd() = default;
    glm::vec3 translate;
    virtual void Push(glm::mat4& mat) override
    {
        mat = glm::translate(mat, translate);
    }
};

struct TransformStack::RotateCmd : public TransformStack::Cmd {
    RotateCmd(float radian, glm::vec3 axis)
        : radian(radian)
        , axis(axis) {};
    virtual ~RotateCmd() = default;
    float radian;
    glm::vec3 axis;
    virtual void Push(glm::mat4& mat) override
    {
        mat = glm::rotate(mat, radian, axis);
    }
};

struct TransformStack::ScaleCmd : public TransformStack::Cmd {
    ScaleCmd(glm::vec3 scale)
        : scale(scale) {};
    virtual ~ScaleCmd() = default;
    glm::vec3 scale;
    virtual void Push(glm::mat4& mat) override
    {
        mat = glm::scale(mat, scale);
    }
};

TransformStack::TransformStack()
{
}

void TransformStack::PushScale(glm::vec3 scale)
{
    m_cmdStack.push_back(std::make_unique<ScaleCmd>(scale));
}

void TransformStack::PushTranslate(glm::vec3 translate)
{
    m_cmdStack.push_back(std::make_unique<TranslateCmd>(translate));
}

void TransformStack::PushRotate(float radian, glm::vec3 axis)
{
    m_cmdStack.push_back(std::make_unique<RotateCmd>(radian, axis));
}

void TransformStack::PushScale(float x, float y, float z)
{
    TransformStack::PushScale(glm::vec3(x, y, z));
}

void TransformStack::PushTranslate(float x, float y, float z)
{
    TransformStack::PushTranslate(glm::vec3(x, y, z));
}

void TransformStack::Apply(std::vector<glm::vec3>& positions)
{
    auto mat = GenerateMatrix();
    for (auto& p : positions) {
        p = glm::vec3(mat * glm::vec4(p, 1.0f));
    }
}

void TransformStack::Pop()
{
    if (m_cmdStack.empty()) {
        return;
    }

    m_cmdStack.pop_back();
}

void TransformStack::Clear()
{
    m_cmdStack.clear();
}

glm::mat4 TransformStack::GenerateMatrix()
{
    glm::mat4 st(1.0f);
    for (auto const& cmd : m_cmdStack) {
        cmd->Push(st);
    }
    return st;
}
