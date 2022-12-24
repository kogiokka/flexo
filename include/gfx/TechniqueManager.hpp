#ifndef TECHNIQUE_MANAGER_H
#define TECHNIQUE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "gfx/Technique.hpp"

// Cross-projects singleton
class TechniqueManager
{
public:
    template <typename... Params>
    static std::shared_ptr<Technique> Resolve(std::string id)
    {
        return Get().ResolveInternal(id);
    }

private:
    TechniqueManager()
    {
    }

    template <typename... Params>
    std::shared_ptr<Technique> ResolveInternal(std::string const& id)
    {
        auto it = m_techniques.find(id);
        if (it != m_techniques.end()) {
            return it->second;
        } else {
            auto tech = std::make_shared<Technique>(id);
            m_techniques[id] = tech;
            return tech;
        }
    }

    static TechniqueManager& Get()
    {
        static TechniqueManager techmgr;
        return techmgr;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Technique>> m_techniques;
};

#endif
