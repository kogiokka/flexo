#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "gfx/BindStep.hpp"
#include "gfx/UniformBlock.hpp"
#include "util/Logger.h"

class Graphics;
class Renderer;
class DrawableBase;

// Collection of BindSteps for rendering a drawable
class Technique
{
public:
    explicit Technique(std::string id);
    void Submit(Renderer& renderer, DrawableBase& drawable);
    void AddBindStep(BindStep step);
    void UpdateUniform(Graphics& gfx, std::string const& id, UniformBlock const& block);
    void Update(Graphics& gfx);
    std::string GetID() const;

private:
    std::string m_id;
    std::vector<BindStep> m_steps;
};

#endif
