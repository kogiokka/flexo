#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>

#include "Attachable.hpp"
#include "Graphics.hpp"
#include "Mesh.hpp"
#include "drawable/Drawable.hpp"
#include "drawable/LatticeEdge.hpp"
#include "drawable/LatticeFace.hpp"
#include "drawable/LatticeVertex.hpp"
#include "drawable/LightSource.hpp"
#include "drawable/PolygonalModel.hpp"
#include "drawable/VolumetricModel.hpp"

class WatermarkingProject;

class Renderer : public AttachableBase
{
public:
    static Renderer& Get(WatermarkingProject& project);
    static Renderer const& Get(WatermarkingProject const& project);

    Renderer(int width, int height);
    void Render();
    void SetCameraView(BoundingBox const& box);
    void LoadLattice(Mesh const& vertexMesh, Mesh const& perVertexData, Mesh const& latticeMesh,
                     std::vector<unsigned int> const& indices);
    void LoadPolygonalModel(Mesh const& mesh);
    void LoadVolumetricModel(Mesh const& instanceMesh, Mesh const& perInstanceData);
    BoundingBox CalculateBoundingBox(std::vector<glm::vec3> positions);
    Camera& GetCamera();
    std::vector<std::shared_ptr<DrawableBase>> const& GetDrawables() const;

private:
    Graphics m_gfx;
    std::vector<std::shared_ptr<DrawableBase>> m_objects;
};

#endif
