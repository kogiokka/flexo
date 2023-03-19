#ifndef DRAW_TASK_H
#define DRAW_TASK_H

#include <memory>
#include <vector>

#include "gfx/BindStep.hpp"

class Graphics;
class DrawableBase;

// A struct reprensents a drawcall for Renderer
struct DrawTask {
    BindStep const* step;
    DrawableBase* drawable; // TODO Make it const

    void Execute(Graphics& gfx);
};

#endif
