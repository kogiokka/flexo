#include <cstdlib>

#include "Renderer.hpp"
#include "Shader.hpp"
#include "World.hpp"
#include "common/Logger.hpp"

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
    vao_.AddAttribFormat(VertexAttrib_TextureCoordintes, { 2, GL_FLOAT, GL_FALSE });
    vao_.AddAttribFormat(VertexAttrib_Instanced, format);
    vao_.Enable(VertexAttrib_Position);
    vao_.Enable(VertexAttrib_Normal);
    vao_.Enable(VertexAttrib_TextureCoordintes);

    glGenBuffers(buffers_.size(), buffers_.data());

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Cube]);
    glBufferData(GL_ARRAY_BUFFER, world.cube.Vertices().size() * sizeof(Vertex), world.cube.Vertices().data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_UVSphere]);
    glBufferData(GL_ARRAY_BUFFER, world.uvsphere.Vertices().size() * sizeof(Vertex), world.uvsphere.Vertices().data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
    glBufferData(GL_ARRAY_BUFFER, world.latticeMesh.faces.size() * sizeof(Vertex2), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, world.latticeMesh.positions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.latticeMesh.indices.size() * sizeof(unsigned int),
                 world.latticeMesh.indices.data(), GL_STATIC_DRAW);

    shaders_[ShaderType_Default].Attach(GL_VERTEX_SHADER, "shader/default.vert");
    shaders_[ShaderType_Default].Attach(GL_FRAGMENT_SHADER, "shader/default.frag");

    shaders_[ShaderType_LatticeVertex].Attach(GL_VERTEX_SHADER, "shader/VertexModel.vert");
    shaders_[ShaderType_LatticeVertex].Attach(GL_FRAGMENT_SHADER, "shader/VertexModel.frag");

    shaders_[ShaderType_LatticeEdge].Attach(GL_VERTEX_SHADER, "shader/Edge.vert");
    shaders_[ShaderType_LatticeEdge].Attach(GL_FRAGMENT_SHADER, "shader/Edge.frag");

    shaders_[ShaderType_LatticeFace].Attach(GL_VERTEX_SHADER, "shader/Texture.vert");
    shaders_[ShaderType_LatticeFace].Attach(GL_FRAGMENT_SHADER, "shader/Texture.frag");

    shaders_[ShaderType_LightSource].Attach(GL_VERTEX_SHADER, "shader/LightSource.vert");
    shaders_[ShaderType_LightSource].Attach(GL_FRAGMENT_SHADER, "shader/LightSource.frag");

    shaders_[ShaderType_VolumetricModel].Attach(GL_VERTEX_SHADER, "shader/FlatShading.vert");
    shaders_[ShaderType_VolumetricModel].Attach(GL_FRAGMENT_SHADER, "shader/FlatShading.frag");

    for (Shader const& s : shaders_) {
        s.Link();
    }

    glGenTextures(1, &tex_);
    glBindTexture(GL_TEXTURE_2D, tex_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    {
        auto const& [img, w, h, ch] = world.pattern;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    }
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Renderer::Render()
{
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    static glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    static auto const vertexScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.02f);
    static auto const lightSrcMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, world.latticeMesh.positions.size() * sizeof(glm::vec3),
                    world.latticeMesh.positions.data());

    Shader* program = nullptr;

    if (rendopt & RenderOption_LightSource) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoordintes);
        vao_.Disable(VertexAttrib_Instanced);
        program = &shaders_[ShaderType_LightSource];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", glm::translate(glm::mat4(1.0f), lightPos) * lightSrcMat);
        program->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_UVSphere], offsetof(Vertex, position),
                           sizeof(Vertex));
        glDrawArrays(GL_TRIANGLES, 0, world.uvsphere.Vertices().size());
    }

    if (rendopt & RenderOption_LatticeVertex) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoordintes);
        vao_.Enable(VertexAttrib_Instanced);
        program = &shaders_[ShaderType_LatticeVertex];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", vertexScaleMat);
        program->SetUniform3fv("viewPos", camera_.Position());
        program->SetUniform3fv("light.position", lightPos);
        program->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
        program->SetUniform3f("light.diffusion", 0.5f, 0.5f, 0.5f);
        program->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.diffusion", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
        program->SetUniform1f("material.shininess", 32.0f);
        program->SetUniform1f("alpha", 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_UVSphere], offsetof(Vertex, position),
                           sizeof(Vertex));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_UVSphere], offsetof(Vertex, normal),
                           sizeof(Vertex));
        glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glVertexBindingDivisor(2, 1);
        glDrawArraysInstanced(GL_TRIANGLES, 0, world.uvsphere.Vertices().size(), world.latticeMesh.positions.size());
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel != nullptr) {
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Disable(VertexAttrib_TextureCoordintes);
            vao_.Disable(VertexAttrib_Instanced);

            program = &shaders_[ShaderType_Default];
            program->Use();
            program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
            program->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
            program->SetUniform3fv("viewPos", camera_.Position());
            program->SetUniform3fv("light.position", lightPos);
            program->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
            program->SetUniform3f("light.diffusion", 0.5f, 0.5f, 0.5f);
            program->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
            program->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
            program->SetUniform3f("material.diffusion", 0.0f, 0.6352941f, 0.9294118f);
            program->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
            program->SetUniform1f("material.shininess", 32.0f);
            program->SetUniform1f("alpha", world.modelColorAlpha);

            glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_Surface], offsetof(Vertex, position),
                               sizeof(Vertex));
            glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_Surface], offsetof(Vertex, normal),
                               sizeof(Vertex));
            glDrawArrays(GL_TRIANGLES, 0, world.polyModel->Vertices().size());
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Disable(VertexAttrib_TextureCoordintes);
            vao_.Enable(VertexAttrib_Instanced);
            program = &shaders_[ShaderType_VolumetricModel];
            program->Use();
            program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
            program->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
            program->SetUniform3fv("viewPos", camera_.Position());
            program->SetUniform3fv("light.position", camera_.Position());

            program->SetUniform3f("light.ambient", 0.8f, 0.8f, 0.8f);
            program->SetUniform3f("light.diffusion", 0.8f, 0.8f, 0.8f);
            program->SetUniform3f("light.specular", 0.8f, 0.8f, 0.8f);

            program->SetUniform3f("material.ambient", 0.0f, 0.3f, 0.0f);
            program->SetUniform3f("material.diffusion", 0.0f, 0.6f, 0.0f);
            program->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
            program->SetUniform1f("material.shininess", 256.0f);

            program->SetUniform1f("alpha", world.modelColorAlpha);

            glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_Cube], offsetof(Vertex, position),
                               sizeof(Vertex));
            glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_Cube], offsetof(Vertex, normal),
                               sizeof(Vertex));
            glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_VolumetricModel], 0, sizeof(glm::vec3));
            glVertexBindingDivisor(2, 1);
            glDrawArraysInstanced(GL_TRIANGLES, 0, world.cube.Vertices().size(), world.volModel->positions.size());
            glDisable(GL_CULL_FACE);
        }
    }

    if (rendopt & RenderOption_LatticeEdge) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoordintes);
        vao_.Disable(VertexAttrib_Instanced);

        program = &shaders_[ShaderType_LatticeEdge];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
        program->SetUniform3f("color", 0.7f, 0.7f, 0.7f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glDrawElements(GL_LINES, world.latticeMesh.indices.size(), GL_UNSIGNED_INT, 0);
    }

    if (rendopt & RenderOption_LatticeFace) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Enable(VertexAttrib_TextureCoordintes);
        vao_.Disable(VertexAttrib_Instanced);

        glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, world.latticeMesh.faces.size() * sizeof(Vertex2), world.latticeMesh.faces.data());

        program = &shaders_[ShaderType_LatticeFace];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
        program->SetUniform3fv("viewPos", camera_.Position());
        program->SetUniform3fv("light.position", lightPos);
        program->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
        program->SetUniform3f("light.diffusion", 0.5f, 0.4f, 0.5f);
        program->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.ambient", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.diffusion", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
        program->SetUniform1f("material.shininess", 32.0f);

        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticeFace], offsetof(Vertex2, position),
                           sizeof(Vertex2));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_LatticeFace], offsetof(Vertex2, normal),
                           sizeof(Vertex2));
        glBindVertexBuffer(VertexAttrib_TextureCoordintes, buffers_[BufferType_LatticeFace], offsetof(Vertex2, texcoord),
                           sizeof(Vertex2));
        glBindTexture(GL_TEXTURE_2D, tex_);
        glDrawArrays(GL_TRIANGLES, 0, world.latticeMesh.faces.size());
    }
}

void Renderer::LoadPolygonalModel()
{
    glDeleteBuffers(1, &buffers_[BufferType_Surface]);
    glGenBuffers(1, &buffers_[BufferType_Surface]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Surface]);
    glBufferData(GL_ARRAY_BUFFER, world.polyModel->Vertices().size() * sizeof(Vertex),
                 world.polyModel->Vertices().data(), GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, world.latticeMesh.faces.size() * sizeof(Vertex2), nullptr, GL_DYNAMIC_DRAW);

    // An Vertex Buffer Object storing the positions of neurons on the world.lattice.tice.
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, world.latticeMesh.positions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // An Index Buffer that holds indices referencing the positions of neurons to draw the edges between them.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.latticeMesh.indices.size() * sizeof(unsigned int),
                 world.latticeMesh.indices.data(), GL_STATIC_DRAW);

    glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
}

void Renderer::LoadVolumetricModel()
{
    glDeleteBuffers(1, &buffers_[BufferType_VolumetricModel]);
    glGenBuffers(1, &buffers_[BufferType_VolumetricModel]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_VolumetricModel]);
    glBufferData(GL_ARRAY_BUFFER, world.volModel->positions.size() * sizeof(glm::vec3),
                 world.volModel->positions.data(), GL_DYNAMIC_DRAW);
    glVertexBindingDivisor(2, 1);
    glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_VolumetricModel], 0, sizeof(glm::vec3));
}

Camera& Renderer::GetCamera()
{
    return camera_;
}
