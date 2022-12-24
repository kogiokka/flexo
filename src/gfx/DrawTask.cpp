#include "gfx/DrawTask.hpp"
#include "gfx/Graphics.hpp"
#include "gfx/drawable/DrawableBase.hpp"

void DrawTask::Execute(Graphics& gfx)
{
    if (drawable->IsVisible()) {
        // InputLayout is responsible for VAO state, so bind the step before any drawable bindings.
        step->Bind(gfx);
        // drawable->Update(gfx);
        drawable->Bind(gfx);
        drawable->Draw(gfx);
    }
}
