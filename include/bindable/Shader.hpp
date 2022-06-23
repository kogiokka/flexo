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
    class Shader : public Bindable
    {
        GLuint id_;

    public:
        Shader(Graphics& gfx);
        void Attach(Graphics& gfx, ShaderStage stage, std::string const& filepath);
        void Link(Graphics& gfx);
        void Bind(Graphics& gfx) override;
    };
}

#endif
