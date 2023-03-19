#include "gfx/DrawTask.hpp"
#include "gfx/DrawableBase.hpp"
#include "gfx/Graphics.hpp"

void DrawTask::Execute(Graphics& gfx)
{
    // InputLayout is responsible for VAO state, so bind the step before any drawable bindings.
    step->Bind(gfx);
    drawable->Update(gfx);
    drawable->Draw(gfx);
}
