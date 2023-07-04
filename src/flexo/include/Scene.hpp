#ifndef DRAW_LIST_H
#define DRAW_LIST_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <wx/event.h>

#include "Attachable.hpp"
#include "VolumetricModelData.hpp"
#include "object/Map.hpp"
#include "object/Object.hpp"

class FlexoProject;
class Renderer;

class Scene : public AttachableBase
{
public:
    static Scene& Get(FlexoProject& project);
    static Scene const& Get(FlexoProject const& project);
    Scene(FlexoProject& project);

    void AddCube(float size = 2.0f);
    void AddPlane(float size = 2.0f);
    void AddUVSphere(int numSegments = 32, int numRings = 16, float radius = 1.0f);
    void AddTorus(int majorSeg = 48, int minorSeg = 12, float majorRad = 1.0f, float minorRad = 0.25f);
    void AddGrid(int numXDiv = 10, int numYDiv = 10, float size = 2.0f);
    void AddMap(int width, int height, MapFlags flags, MapInitState initState);
    void AddModel(VolumetricModelData const& data);
    std::weak_ptr<Object> GetObject(std::string const& id) const;
    std::vector<std::string> GetAllModelsByID() const;
    std::vector<std::string> GetAllMapsByID() const;
    void SubmitDrawables(Renderer& renderer) const;

    void Delete(std::string const& id);

private:
    void AcceptObject(std::shared_ptr<Object> object);

    std::vector<std::shared_ptr<Object>> m_list;
    std::unordered_map<enum ObjectType, unsigned int> m_typeCount;
    FlexoProject& m_project;
};

#endif
