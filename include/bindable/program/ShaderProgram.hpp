#ifndef SHADER_H
#define SHADER_H

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

namespace Bind
{
    class ShaderProgram : public Bindable
    {
        GLuint m_id;

    public:
        ShaderProgram(Graphics& gfx);
        ~ShaderProgram() override;
        void Bind() override;
        void Attach(ShaderStage stage, std::string const& filepath);
        void Link();
    };
}

#endif
