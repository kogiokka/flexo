#ifndef GUIDES_H
#define GUIDES_H

#include <memory>

#include "EditableMesh.hpp"
#include "WireDrawable.hpp"

class Graphics;
class DrawableBase;
class Renderer;

typedef int OverlayFlags;

enum Overlays_ {
    Overlays_GuidesGrid = 1 << 0,
    Overlays_GuidesAxisX = 1 << 1,
    Overlays_GuidesAxisY = 1 << 2,
};

class Overlays
{
public:
    Overlays(Graphics& gfx);
    void Submit(Renderer& renderer);
    void SetFlags(OverlayFlags flags);

private:
    std::shared_ptr<WireDrawable> m_gridFine;
    std::shared_ptr<WireDrawable> m_gridCoarse;
    std::shared_ptr<WireDrawable> m_axisX;
    std::shared_ptr<WireDrawable> m_axisY;

    OverlayFlags m_flags;
};

#endif
