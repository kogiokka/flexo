#include "bindable/Texture.hpp"

template <typename T>
Texture::Texture(Graphics& gfx, std::vector<T> const& texture)
{
    Texture2dDesc desc;
    BufferData data;

    data.mem = texture.data();
    gfx.CreateTexture2D(id_, desc, data);
}

void Texture::Bind(Graphics& gfx) { }
