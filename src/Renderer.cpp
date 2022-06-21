#include "Renderer.hpp"
#include "Shader.hpp"
#include "World.hpp"
#include "common/Logger.hpp"
#include <cstdlib>

RenderOption rendopt = RenderOption_Model | RenderOption_LatticeEdge | RenderOption_LatticeVertex;

Renderer::Renderer(int width, int height)
    : gfx_(width, height)
    , polyModel_(nullptr)
    , lightSource_(nullptr)
    , volModel_(nullptr)
    , latticeFace_(nullptr)
{
    AttribFormat format;
    format.count = 3;
    format.type = GL_FLOAT;
    format.normalized = GL_FALSE;
    vao_.Bind();
    vao_.AddAttribFormat(VertexAttrib_Position, format); // 0
    vao_.AddAttribFormat(VertexAttrib_Normal, format); // 1
    vao_.AddAttribFormat(VertexAttrib_Translation, format); // 2
    vao_.AddAttribFormat(VertexAttrib_TextureCoord, { 2, GL_FLOAT, GL_FALSE }); // 3
    vao_.Enable(VertexAttrib_Position);
    vao_.Enable(VertexAttrib_Normal);
    vao_.Enable(VertexAttrib_TextureCoord);

    vaoTemp_.Bind();
    vaoTemp_.AddAttribFormat(VertexAttrib_Position, format); // 0
    vaoTemp_.AddAttribFormat(VertexAttrib_Normal, format); // 1
    vaoTemp_.AddAttribFormat(2, { 2, GL_FLOAT, GL_FALSE }); // Texture coordinates
    vaoTemp_.Enable(VertexAttrib_Position);
    vaoTemp_.Enable(VertexAttrib_Normal);
    vaoTemp_.Enable(2);

    vao_.Bind();

    glGenBuffers(buffers_.size(), buffers_.data());

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferData(GL_ARRAY_BUFFER, world.neuronPositions.size() * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, world.latticeEdges.size() * sizeof(unsigned int), world.latticeEdges.data(),
                 GL_STATIC_DRAW);

    shaders_[ShaderType_LatticeEdge].Attach(GL_VERTEX_SHADER, "shader/LatticeEdge.vert");
    shaders_[ShaderType_LatticeEdge].Attach(GL_FRAGMENT_SHADER, "shader/LatticeEdge.frag");

    for (Shader const& s : shaders_) {
        s.Link();
    }

    lightSource_ = std::make_unique<LightSource>(gfx_, world.uvsphere);
}

void Renderer::Render()
{
    if (world.polyModel == nullptr && world.volModel == nullptr) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType_LatticePositions]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, world.neuronPositions.size() * sizeof(glm::vec3), world.neuronPositions.data());

    Shader* program = nullptr;

    if (rendopt & RenderOption_LightSource) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Disable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Disable(VertexAttrib_Translation);
        lightSource_->Update(gfx_);
        lightSource_->Draw(gfx_);
    }

    if (rendopt & RenderOption_LatticeVertex) {
        vao_.Enable(VertexAttrib_Position);
        vao_.Enable(VertexAttrib_Normal);
        vao_.Disable(VertexAttrib_TextureCoord);
        vao_.Enable(VertexAttrib_Translation);
        latticeVert_->Update(gfx_);
        latticeVert_->Draw(gfx_);
    }

    if (rendopt & RenderOption_Model) {
        if (world.polyModel && polyModel_) {
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Disable(VertexAttrib_TextureCoord);
            vao_.Disable(VertexAttrib_Translation);
            polyModel_->Update(gfx_);
            polyModel_->Draw(gfx_);
        } else if (world.volModel != nullptr) {
            glEnable(GL_CULL_FACE); // FIXME: VertexArrray bindable?
            glCullFace(GL_BACK);
            vao_.Enable(VertexAttrib_Position);
            vao_.Enable(VertexAttrib_Normal);
            vao_.Enable(VertexAttrib_TextureCoord);
            vao_.Enable(VertexAttrib_Translation);
            volModel_->Update(gfx_);
            volModel_->Draw(gfx_);
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
        program->SetUniformMatrix4fv("ubo.vert.viewProjMat", gfx_.GetCamera().ViewProjectionMatrix());
        program->SetUniformMatrix4fv("ubo.vert.modelMat", glm::mat4(1.0f));
        program->SetUniform3f("ubo.frag.color", 0.7f, 0.7f, 0.7f);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[BufferType_LatticeEdge]);
        glBindVertexBuffer(VertexAttrib_Position, buffers_[BufferType_LatticePositions], 0, sizeof(glm::vec3));
        glDrawElements(GL_LINES, world.latticeEdges.size(), GL_UNSIGNED_INT, 0);
    }

    if (rendopt & RenderOption_LatticeFace) {
        vaoTemp_.Bind();
        latticeFace_->Update(gfx_);
        latticeFace_->Draw(gfx_);
        vao_.Bind();
    }
}

void Renderer::LoadPolygonalModel()
{
    assert(world.polyModel);
    polyModel_ = std::make_unique<PolygonalModel>(gfx_, *world.polyModel);
}

void Renderer::LoadLattice()
{
    latticeVert_ = std::make_unique<LatticeVertex>(gfx_, world.uvsphere);
    latticeFace_ = std::make_unique<LatticeFace>(gfx_, world.latticeMesh);

    glDeleteBuffers(1, &buffers_[BufferType_LatticeEdge]);
    glDeleteBuffers(1, &buffers_[BufferType_LatticePositions]);
    glGenBuffers(1, &buffers_[BufferType_LatticeEdge]);
    glGenBuffers(1, &buffers_[BufferType_LatticePositions]);

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
    volModel_ = std::make_unique<VolumetricModel>(gfx_, world.cube);
}

Camera& Renderer::GetCamera()
{
    return gfx_.GetCamera();
}
