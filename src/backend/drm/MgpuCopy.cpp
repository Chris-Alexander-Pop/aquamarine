#include "MgpuCopy.hpp"

using namespace Hyprutils::Math;

bool Aquamarine::damageCoversBuffer(const CRegion& damage, const Vector2D& size) {
    if (size.x <= 0 || size.y <= 0)
        return true;

    const auto ext = damage.copy().getExtents();
    return ext.x <= 0 && ext.y <= 0 && ext.w >= size.x && ext.h >= size.y;
}

Aquamarine::SMgpuCopyDecision Aquamarine::decideMgpuCopy(bool haveDamage, bool damageEmpty, bool coversBuffer, uint32_t primed, size_t chainLength) {
    SMgpuCopyDecision decision;
    if (!haveDamage)
        return decision;

    if (damageEmpty) {
        // An empty region is "nothing changed", but only after every slot in
        // the scanout chain has been filled. chainLength 0 never skips.
        if (chainLength > 0 && primed >= chainLength)
            decision.skip = true;
        return decision;
    }

    // chainLength 0 means the swapchain is not set up. A partial copy would
    // leave uninitialized pixels, so copy the whole buffer.
    if (chainLength == 0 || primed < chainLength || coversBuffer)
        return decision;

    decision.full = false;
    return decision;
}

Aquamarine::SGlScissor Aquamarine::glScissorForTopLeftRect(int x1, int y1, int x2, int y2, int bufW, int bufH) {
    SGlScissor box;
    if (bufW <= 0 || bufH <= 0)
        return box;

    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x2 > bufW)
        x2 = bufW;
    if (y2 > bufH)
        y2 = bufH;
    if (x2 <= x1 || y2 <= y1)
        return box;

    box.x    = x1;
    box.y    = bufH - y2;
    box.w    = x2 - x1;
    box.h    = y2 - y1;
    box.draw = box.w > 0 && box.h > 0 && box.x >= 0 && box.y >= 0;
    return box;
}
