#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "gfx/Graphics.hpp"
#include "gfx/bindable/Texture2D.hpp"

namespace Bind
{
    class TextureManager
    {
    public:
        template <typename... Params>
        static std::shared_ptr<Texture2D> Resolve(Graphics& gfx, Params&&... params)
        {
            return Get().ResolveInternal(gfx, std::forward<Params>(params)...);
        }

    private:
        TextureManager()
        {
        }

        template <typename... Params>
        std::shared_ptr<Texture2D> ResolveInternal(Graphics& gfx, Params&&... params)
        {
            auto const key = Texture2D::GenerateUID(std::forward<Params>(params)...);
            auto it = m_binds.find(key);
            if (it != m_binds.end()) {
                return it->second;
            } else {
                auto bindable = std::make_shared<Texture2D>(gfx, std::forward<Params>(params)...);
                m_binds[key] = bindable;
                return bindable;
            }
        }

        static TextureManager& Get()
        {
            static TextureManager texmgr;
            return texmgr;
        }

    private:
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_binds;
    };
}

#endif
