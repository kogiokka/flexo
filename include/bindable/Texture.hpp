#ifndef TEXTURE_H
#define TEXTURE_H

#include "Graphics.hpp"
#include "bindable/Bindable.hpp"

#include <vector>

class Texture : public Bindable
{
protected:
    GLuint id_;

public:
    template <typename T>
    Texture(Graphics& gfx, std::vector<T> const& texture);
    void Bind(Graphics& gfx) override;
};

#endif
