#include "Renderer.hpp"
#include "Shader.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include "drawable/UVSphere.hpp"
#include <cstdlib>

struct Light {
    alignas(sizeof(glm::vec4)) glm::vec3 position;
    alignas(sizeof(glm::vec4)) glm::vec3 ambient;
    alignas(sizeof(glm::vec4)) glm::vec3 diffusion;
    alignas(sizeof(glm::vec4)) glm::vec3 specular;
};

struct Material {
    alignas(sizeof(glm::vec4)) glm::vec3 ambient;
    alignas(sizeof(glm::vec4)) glm::vec3 diffusion;
    alignas(sizeof(glm::vec4)) glm::vec3 specular;
    float shininess;
};

struct UniformBuffer_Vert {
    glm::mat4 viewProjMat;
    glm::mat4 modelMat;
};

struct UniformBuffer_Frag {
    Light light;
    Material material;
    alignas(sizeof(glm::vec4)) glm::vec3 viewPos;
    float alpha;
};

struct UniformBuffer {
    UniformBuffer_Vert vert;
    UniformBuffer_Frag frag;
};

GLuint ubo;
GLuint uniformBlockIndex;
GLuint uniformBlockBinding = 0;

UniformBuffer ubdata;

RenderOption rendopt = RenderOption_Model | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

Renderer::Renderer(int width, int height)
    : camera_(width, height)
{
    AttribFormat format;
    format.count = 3;
    format.type = GL_FLOAT;
    format.normalized = GL_FALSE;
    vao_.Bind();
    vao_.AddAttribFormat(VertexAttrib_Position, format);
    vao_.AddAttribFormat(VertexAttrib_Normal, format);
    vao_.AddAttribFormat(VertexAttrib_TextureCoord, { 2, GL_FLOAT, GL_FALSE });
    vao_.AddAttribFormat(VertexAttrib_Translation, format);
    vao_.Enable(VertexAttrib_Position);
    vao_.Enable(VertexAttrib_Normal);
    vao_.Enable(VertexAttrib_TextureCoord);

    drawables_.push_back(std::make_unique<UVSphere>(gfx_));

    CreateVertexBuffers();

    glGenBuffers(buffers_.size(), buffers_.data());

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Cube]);
    glBufferData(GL_ARRAY_BUFFER, cubeBuf_.size() * sizeof(VertexPN), cubeBuf_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_UVSphere]);
    glBufferData(GL_ARRAY_BUFFER, uvsphereBuf_.size() * sizeof(VertexPN), uvsphereBuf_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
    glBufferData(GL_ARRAY_BUFFER, latticeMeshBuf_.size() * sizeof(VertexPNT), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, world.neuronPositions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.latticeEdges.size() * sizeof(unsigned int), world.latticeEdges.data(),
                 GL_STATIC_DRAW);

    shaders_[ShaderType_PolygonalModel].Attach(GL_VERTEX_SHADER, "shader/PolygonalModel.vert");
    shaders_[ShaderType_PolygonalModel].Attach(GL_FRAGMENT_SHADER, "shader/PolygonalModel.frag");

    shaders_[ShaderType_LatticeVertex].Attach(GL_VERTEX_SHADER, "shader/LatticeVertex.vert");
    shaders_[ShaderType_LatticeVertex].Attach(GL_FRAGMENT_SHADER, "shader/LatticeVertex.frag");

    shaders_[ShaderType_LatticeEdge].Attach(GL_VERTEX_SHADER, "shader/LatticeEdge.vert");
    shaders_[ShaderType_LatticeEdge].Attach(GL_FRAGMENT_SHADER, "shader/LatticeEdge.frag");

    shaders_[ShaderType_LatticeFace].Attach(GL_VERTEX_SHADER, "shader/LatticeFace.vert");
    shaders_[ShaderType_LatticeFace].Attach(GL_FRAGMENT_SHADER, "shader/LatticeFace.frag");

    shaders_[ShaderType_LightSource].Attach(GL_VERTEX_SHADER, "shader/LightSource.vert");
    shaders_[ShaderType_LightSource].Attach(GL_FRAGMENT_SHADER, "shader/LightSource.frag");

    shaders_[ShaderType_VolumetricModel].Attach(GL_VERTEX_SHADER, "shader/VolumetricModel.vert");
    shaders_[ShaderType_VolumetricModel].Attach(GL_FRAGMENT_SHADER, "shader/VolumetricModel.frag");

    for (Shader const& s : shaders_) {
        s.Link();
    }

    // Test UniformBuffer
    uniformBlockIndex = glGetUniformBlockIndex(shaders_[ShaderType_PolygonalModel].Id(), "UniformBuffer");
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UniformBuffer), nullptr, GL_STREAM_DRAW);
    glUniformBlockBinding(shaders_[ShaderType_PolygonalModel].Id(), uniformBlockIndex, uniformBlockBinding);
    glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, ubo);
    glBindBufferRange(GL_UNIFORM_BUFFER, uniformBlockIndex, ubo, 0, sizeof(UniformBuffer));

    glGenTextures(1, &tex_);
    glBindTexture(GL_TEXTURE_2D, tex_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    {
        auto const& [img, w, h, ch] = world.pattern;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    }

    glGenTextures(1, &texColor_);
    glBindTexture(GL_TEXTURE_2D, texColor_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    {
        float color[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, color);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    // Render the volumetric model with a color at the beginning
    texVolModel_ = texColor_;
}

void Renderer::Render()
{
    //     for (auto const& d : drawables_) {
    //         d->Draw(gfx_);
    //     }
    //
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    static glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    static auto const vertexScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.02f);
    static auto const lightSrcMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, world.neuronPositions.size() * sizeof(glm::vec3), world.neuronPositions.data());

    Shader* program = nullptr;

    if (rendopt & RenderOption_LightSource) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);
        program = &shaders_[ShaderType_LightSource];
        program->Use();
        program->SetUniformMatrix4fv("ubo.vert.viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("ubo.vert.modelMat", glm::translate(glm::mat4(1.0f), lightPos) * lightSrcMat);
        program->SetUniform3f("ubo.frag.lightColor", 1.0f, 1.0f, 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_UVSphere], offsetof(VertexPN, position),
                           sizeof(VertexPN));
        glDrawArrays(GL_TRIANGLES, 0, uvsphereBuf_.size());
    }

    if (rendopt & RenderOption_LatticeVertex) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Enable(VertexAttrib_Translation);
        program = &shaders_[ShaderType_LatticeVertex];
        program->Use();
        program->SetUniformMatrix4fv("ubo.vert.viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("ubo.vert.modelMat", vertexScaleMat);
        program->SetUniform3fv("ubo.frag.light.position", lightPos);
        program->SetUniform3f("ubo.frag.light.ambient", 0.2f, 0.2f, 0.2f);
        program->SetUniform3f("ubo.frag.light.diffusion", 0.5f, 0.5f, 0.5f);
        program->SetUniform3f("ubo.frag.light.specular", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.ambient", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.diffusion", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.specular", 0.3f, 0.3f, 0.3f);
        program->SetUniform1f("ubo.frag.material.shininess", 32.0f);
        program->SetUniform3fv("ubo.frag.viewPos", camera_.Position());
        program->SetUniform1f("ubo.frag.alpha", 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_UVSphere], offsetof(VertexPN, position),
                           sizeof(VertexPN));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_UVSphere], offsetof(VertexPN, normal),
                           sizeof(VertexPN));
        glBindVertexBuffer(VertexAttrib_Translation, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glVertexBindingDivisor(VertexAttrib_Translation, 1);
        glDrawArraysInstanced(GL_TRIANGLES, 0, uvsphereBuf_.size(), world.neuronPositions.size());
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel != nullptr) {
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Disable(VertexAttrib_TextureCoord);
            vao_.Disable(VertexAttrib_Translation);

            program = &shaders_[ShaderType_PolygonalModel];
            program->Use();
            ubdata.vert.viewProjMat = camera_.ViewProjectionMatrix();
            ubdata.vert.modelMat = glm::mat4(1.0f);
            ubdata.frag.light.position = lightPos;
            ubdata.frag.light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
            ubdata.frag.light.diffusion = glm::vec3(0.5f, 0.5f, 0.5f);
            ubdata.frag.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
            ubdata.frag.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
            ubdata.frag.material.diffusion = glm::vec3(0.0f, 0.6352941f, 0.9294118f);
            ubdata.frag.material.specular = glm::vec3(0.3f, 0.3f, 0.3f);
            ubdata.frag.material.shininess = 32.0f;
            ubdata.frag.viewPos = camera_.Position();
            ubdata.frag.alpha = world.modelColorAlpha;

            glBindBufferBase(GL_UNIFORM_BUFFER, uniformBlockBinding, ubo);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UniformBuffer), &ubdata);

            glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_Surface], offsetof(VertexPN, position),
                               sizeof(VertexPN));
            glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_Surface], offsetof(VertexPN, normal),
                               sizeof(VertexPN));
            glDrawArrays(GL_TRIANGLES, 0, polyModelBuf_.size());
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Enable(VertexAttrib_TextureCoord);
            vao_.Enable(VertexAttrib_Translation);
            program = &shaders_[ShaderType_VolumetricModel];
            program->Use();
            program->SetUniformMatrix4fv("ubo.vert.viewProjMat", camera_.ViewProjectionMatrix());
            program->SetUniformMatrix4fv("ubo.vert.modelMat", glm::mat4(1.0f));
            program->SetUniform3fv("ubo.vert.viewPos", camera_.Position());
            program->SetUniform3fv("ubo.vert.light.position", camera_.Position());

            program->SetUniform3f("ubo.vert.light.ambient", 0.8f, 0.8f, 0.8f);
            program->SetUniform3f("ubo.vert.light.diffusion", 0.8f, 0.8f, 0.8f);
            program->SetUniform3f("ubo.vert.light.specular", 0.8f, 0.8f, 0.8f);

            program->SetUniform3f("ubo.vert.material.ambient", 0.3f, 0.3f, 0.3f);
            program->SetUniform3f("ubo.vert.material.diffusion", 0.6f, 0.6f, 0.6f);
            program->SetUniform3f("ubo.vert.material.specular", 0.3f, 0.3f, 0.3f);
            program->SetUniform1f("ubo.vert.material.shininess", 256.0f);

            program->SetUniform1f("ubo.vert.alpha", world.modelColorAlpha);

            glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_Cube], offsetof(VertexPN, position),
                               sizeof(VertexPN));
            glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_Cube], offsetof(VertexPN, normal),
                               sizeof(VertexPN));
            glBindVertexBuffer(VertexAttrib_Translation, buffers_[BufferType_VolumetricModel_Translation], 0,
                               sizeof(glm::vec3));
            glBindVertexBuffer(VertexAttrib_TextureCoord, buffers_[BufferType_VolumetricModel_TextureCoord], 0,
                               sizeof(glm::vec2));
            glVertexBindingDivisor(VertexAttrib_Translation, 1);
            glVertexBindingDivisor(VertexAttrib_TextureCoord, 1);
            glBindTexture(GL_TEXTURE_2D, texVolModel_);
            glDrawArraysInstanced(GL_TRIANGLES, 0, cubeBuf_.size(), world.volModel->positions.size());
            glDisable(GL_CULL_FACE);
        }
    }

    if (rendopt & RenderOption_LatticeEdge) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);

        program = &shaders_[ShaderType_LatticeEdge];
        program->Use();
        program->SetUniformMatrix4fv("ubo.vert.viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("ubo.vert.modelMat", glm::mat4(1.0f));
        program->SetUniform3f("ubo.frag.color", 0.7f, 0.7f, 0.7f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glDrawElements(GL_LINES, world.latticeEdges.size(), GL_UNSIGNED_INT, 0);
    }

    if (rendopt & RenderOption_LatticeFace) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Enable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);

        UpdateLatticeMeshBuffer();
        glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, latticeMeshBuf_.size() * sizeof(VertexPNT), latticeMeshBuf_.data());

        program = &shaders_[ShaderType_LatticeFace];
        program->Use();
        program->SetUniformMatrix4fv("ubo.vert.viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("ubo.vert.modelMat", glm::mat4(1.0f));
        program->SetUniform3fv("ubo.frag.viewPos", camera_.Position());
        program->SetUniform3fv("ubo.frag.light.position", lightPos);
        program->SetUniform3f("ubo.frag.light.ambient", 0.2f, 0.2f, 0.2f);
        program->SetUniform3f("ubo.frag.light.diffusion", 0.5f, 0.4f, 0.5f);
        program->SetUniform3f("ubo.frag.light.specular", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.ambient", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.diffusion", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("ubo.frag.material.specular", 0.3f, 0.3f, 0.3f);
        program->SetUniform1f("ubo.frag.material.shininess", 32.0f);

        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticeFace], offsetof(VertexPNT, position),
                           sizeof(VertexPNT));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_LatticeFace], offsetof(VertexPNT, normal),
                           sizeof(VertexPNT));
        glBindVertexBuffer(VertexAttrib_TextureCoord, buffers_[BufferType_LatticeFace], offsetof(VertexPNT, texcoord),
                           sizeof(VertexPNT));
        glVertexBindingDivisor(VertexAttrib_TextureCoord, 0);
        glBindTexture(GL_TEXTURE_2D, tex_);
        glDrawArrays(GL_TRIANGLES, 0, latticeMeshBuf_.size());
    }
}

void Renderer::LoadPolygonalModel()
{
    assert(world.polyModel);

    polyModelBuf_.clear();
    for (unsigned int i = 0; i < world.polyModel->positions.size(); i++) {
        VertexPN v;
        v.position = world.polyModel->positions[i];
        v.normal = world.polyModel->normals[i];
        polyModelBuf_.push_back(v);
    }

    glDeleteBuffers(1, &buffers_[BufferType_Surface]);
    glGenBuffers(1, &buffers_[BufferType_Surface]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Surface]);
    glBufferData(GL_ARRAY_BUFFER, polyModelBuf_.size() * sizeof(VertexPN), polyModelBuf_.data(), GL_STATIC_DRAW);
}

void Renderer::LoadLattice()
{
    glDeleteBuffers(1, &buffers_[BufferType_LatticeEdge]);
    glDeleteBuffers(1, &buffers_[BufferType_LatticeFace]);
    glDeleteBuffers(1, &buffers_[BufferType_LatticePositions]);
    glGenBuffers(1, &buffers_[BufferType_LatticeEdge]);
    glGenBuffers(1, &buffers_[BufferType_LatticeFace]);
    glGenBuffers(1, &buffers_[BufferType_LatticePositions]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
    glBufferData(GL_ARRAY_BUFFER, latticeMeshBuf_.size() * sizeof(VertexPNT), nullptr, GL_DYNAMIC_DRAW);

    // An Vertex Buffer Object storing the positions of neurons on the world.lattice.tice.
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, world.neuronPositions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // An Index Buffer that holds indices referencing the positions of neurons to draw the edges between them.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.latticeEdges.size() * sizeof(unsigned int), world.latticeEdges.data(),
                 GL_STATIC_DRAW);

    glBindVertexBuffer(VertexAttrib_Translation, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
}

void Renderer::LoadVolumetricModel()
{
    glDeleteBuffers(1, &buffers_[BufferType_VolumetricModel_Translation]);
    glDeleteBuffers(1, &buffers_[BufferType_VolumetricModel_TextureCoord]);

    glGenBuffers(1, &buffers_[BufferType_VolumetricModel_Translation]);
    glGenBuffers(1, &buffers_[BufferType_VolumetricModel_TextureCoord]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_VolumetricModel_Translation]);
    glBufferData(GL_ARRAY_BUFFER, world.volModel->positions.size() * sizeof(glm::vec3),
                 world.volModel->positions.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_VolumetricModel_TextureCoord]);
    glBufferData(GL_ARRAY_BUFFER, world.volModel->textureCoords.size() * sizeof(glm::vec2),
                 world.volModel->textureCoords.data(), GL_DYNAMIC_DRAW);

    glVertexBindingDivisor(VertexAttrib_Translation, 1);
    glVertexBindingDivisor(VertexAttrib_TextureCoord, 1);
    glBindVertexBuffer(VertexAttrib_Translation, buffers_[BufferType_VolumetricModel_Translation], 0,
                       sizeof(glm::vec3));
    glBindVertexBuffer(VertexAttrib_TextureCoord, buffers_[BufferType_VolumetricModel_TextureCoord], 0,
                       sizeof(glm::vec2));
}

Camera& Renderer::GetCamera()
{
    return camera_;
}

void Renderer::UpdateLatticeMeshBuffer()
{
    latticeMeshBuf_.clear();
    latticeMeshBuf_.resize(world.latticeMesh.positions.size());
    for (unsigned int i = 0; i < world.latticeMesh.positions.size(); i++) {
        VertexPNT v;
        v.position = world.latticeMesh.positions[i];
        v.normal = world.latticeMesh.normals[i];
        v.texcoord = world.latticeMesh.textureCoords[i];
        latticeMeshBuf_[i] = v;
    }
}

void Renderer::SetWatermarkTexture()
{
    glDeleteBuffers(1, &buffers_[BufferType_VolumetricModel_TextureCoord]);
    glGenBuffers(1, &buffers_[BufferType_VolumetricModel_TextureCoord]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_VolumetricModel_TextureCoord]);
    glBufferData(GL_ARRAY_BUFFER, world.volModel->textureCoords.size() * sizeof(glm::vec2),
                 world.volModel->textureCoords.data(), GL_DYNAMIC_DRAW);

    texVolModel_ = tex_;
}

void Renderer::CreateVertexBuffers()
{
    for (unsigned int i = 0; i < world.cube.positions.size(); i++) {
        VertexPN v;
        v.position = world.cube.positions[i];
        v.normal = world.cube.normals[i];
        cubeBuf_.push_back(v);
    }

    for (unsigned int i = 0; i < world.uvsphere.positions.size(); i++) {
        VertexPN v;
        v.position = world.uvsphere.positions[i];
        v.normal = world.uvsphere.normals[i];
        uvsphereBuf_.push_back(v);
    }

    UpdateLatticeMeshBuffer();
}
