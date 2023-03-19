#ifndef INDEXED_DRAWABLE_H
#define INDEXED_DRAWABLE_H

#include "Drawable.hpp"

class Graphics;

class IndexedDrawable : public Drawable
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
