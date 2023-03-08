#ifndef INDEXED_DRAWABLE_H
#define INDEXED_DRAWABLE_H

#include <vector>

#include "gfx/drawable/DrawableBase.hpp"

class Graphics;

class IndexedDrawable : public DrawableBase
{
public:
    IndexedDrawable();
    IndexedDrawable(IndexedDrawable const&) = delete;
    virtual ~IndexedDrawable() = default;

    virtual void Draw(Graphics& gfx) const override;

protected:
    unsigned int m_indexCount;
};

#endif
