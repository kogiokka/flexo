#include "Renderer.hpp"
#include "Shader.hpp"

std::vector<glm::vec3> latPos;
std::vector<Vertex> latFace;
std::vector<unsigned int> latticeEdgeIndices;
float surfaceColorAlpha = 0.8f;

RenderOption rendopt = RenderOption_Surface | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

Renderer::Renderer(int width, int height)
    : width_(width)
    , height_(height)
    , camera_(width, height)
{
    AttribFormat format;
    format.count = 3;
    format.type = GL_FLOAT;
    format.normalized = GL_FALSE;
    vao_.Bind();
    vao_.AddAttribFormat(VertexAttrib_Position, format);
    vao_.AddAttribFormat(VertexAttrib_Normal, format);
    vao_.AddAttribFormat(VertexAttrib_Instanced, format);
    vao_.Enable(VertexAttrib_Position);
    vao_.Enable(VertexAttrib_Normal);

    glGenBuffers(buffers_.size(), buffers_.data());

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Surface]);
    glBufferData(GL_ARRAY_BUFFER, surface.Vertices().size() * sizeof(Vertex), surface.Vertices().data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_UVSphere]);
    glBufferData(GL_ARRAY_BUFFER, uvsphere.Vertices().size() * sizeof(Vertex), uvsphere.Vertices().data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
    glBufferData(GL_ARRAY_BUFFER, latFace.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // An Vertex Buffer Object storing the positions of neurons on the lattice.
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, latPos.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // An Index Buffer that holds indices referencing the positions of neurons to draw the edges between them.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, latticeEdgeIndices.size() * sizeof(unsigned int), latticeEdgeIndices.data(),
                 GL_STATIC_DRAW);

    glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
    glVertexBindingDivisor(2, 1);

    shaders_[ShaderType_Default].Attach(GL_VERTEX_SHADER, "shader/default.vert");
    shaders_[ShaderType_Default].Attach(GL_FRAGMENT_SHADER, "shader/default.frag");

    shaders_[ShaderType_LatticeEdge].Attach(GL_VERTEX_SHADER, "shader/Edge.vert");
    shaders_[ShaderType_LatticeEdge].Attach(GL_FRAGMENT_SHADER, "shader/Edge.frag");

    shaders_[ShaderType_LatticeVertex].Attach(GL_VERTEX_SHADER, "shader/VertexModel.vert");
    shaders_[ShaderType_LatticeVertex].Attach(GL_FRAGMENT_SHADER, "shader/VertexModel.frag");

    shaders_[ShaderType_LightSource].Attach(GL_VERTEX_SHADER, "shader/LightSource.vert");
    shaders_[ShaderType_LightSource].Attach(GL_FRAGMENT_SHADER, "shader/LightSource.frag");

    for (Shader const& s : shaders_) {
        s.Link();
    }
}

void Renderer::Render()
{
    static glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
    static auto const vertexScaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.02f);
    static auto const lightSrcMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, latPos.size() * sizeof(glm::vec3), latPos.data());

    Shader* program = nullptr;

    if (rendopt & RenderOption_LightSource) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_Instanced);
        program = &shaders_[ShaderType_LightSource];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", glm::translate(glm::mat4(1.0f), lightPos) * lightSrcMat);
        program->SetUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_UVSphere], offsetof(Vertex, position),
                           sizeof(Vertex));
        glDrawArrays(GL_TRIANGLES, 0, uvsphere.Vertices().size());
    }

    if (rendopt & RenderOption_LatticeVertex) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
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
        glDrawArraysInstanced(GL_TRIANGLES, 0, uvsphere.Vertices().size(), latPos.size());
    }

    if (rendopt & RenderOption_LatticeFace) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_Instanced);

        glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticeFace]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, latFace.size() * sizeof(Vertex), latFace.data());

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
        program->SetUniform3f("material.diffusion", 1.0f, 1.0f, 1.0f);
        program->SetUniform3f("material.specular", 0.3f, 0.3f, 0.3f);
        program->SetUniform1f("material.shininess", 32.0f);
        program->SetUniform1f("alpha", 1.0f);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticeFace], offsetof(Vertex, position),
                           sizeof(Vertex));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_LatticeFace], offsetof(Vertex, normal),
                           sizeof(Vertex));
        glDrawArrays(GL_TRIANGLES, 0, latFace.size());
    }

    if (rendopt & RenderOption_Surface) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
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
        program->SetUniform1f("alpha", surfaceColorAlpha);

        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_Surface], offsetof(Vertex, position),
                           sizeof(Vertex));
        glBindVertexBuffer(VertexAttrib_Normal, buffers_[BufferType_Surface], offsetof(Vertex, normal), sizeof(Vertex));
        glDrawArrays(GL_TRIANGLES, 0, surface.Vertices().size());
    }

    if (rendopt & RenderOption_LatticeEdge) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_Instanced);

        program = &shaders_[ShaderType_LatticeEdge];
        program->Use();
        program->SetUniformMatrix4fv("viewProjMat", camera_.ViewProjectionMatrix());
        program->SetUniformMatrix4fv("modelMat", glm::mat4(1.0f));
        program->SetUniform3f("color", 0.7f, 0.7f, 0.7f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glDrawElements(GL_LINES, latticeEdgeIndices.size(), GL_UNSIGNED_INT, 0);
    }
}

void Renderer::LoadSurface()
{
    glDeleteBuffers(1, &buffers_[BufferType_Surface]);
    glGenBuffers(1, &buffers_[BufferType_Surface]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_Surface]);
    glBufferData(GL_ARRAY_BUFFER, surface.Vertices().size() * sizeof(Vertex), surface.Vertices().data(),
                 GL_STATIC_DRAW);
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
    glBufferData(GL_ARRAY_BUFFER, latFace.size() * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    // An Vertex Buffer Object storing the positions of neurons on the lattice.
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, latPos.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    // An Index Buffer that holds indices referencing the positions of neurons to draw the edges between them.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, latticeEdgeIndices.size() * sizeof(unsigned int), latticeEdgeIndices.data(),
                 GL_STATIC_DRAW);

    glBindVertexBuffer(VertexAttrib_Instanced, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
}

Camera& Renderer::GetCamera()
{
    return camera_;
}
