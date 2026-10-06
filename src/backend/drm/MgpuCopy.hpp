#pragma once

#include <hyprutils/math/Region.hpp>

#include <cstddef>
#include <cstdint>

namespace Aquamarine {
    // What the scanout blit should do for one frame. `skip` means the picture
    // did not change and every scanout slot already holds a full image.
    // `full` means copy the whole buffer. Both false means scissor to damage.
    struct SMgpuCopyDecision {
        bool skip = false;
        bool full = true;
    };

    // True when the damage box covers the whole buffer, or when the buffer
    // size is nonsense and a scissor would be a bad idea.
    bool damageCoversBuffer(const Hyprutils::Math::CRegion& damage, const Hyprutils::Math::Vector2D& size);

    // haveDamage is false when the commit did not carry damage, or there is no
    // buffer. Those frames copy the whole buffer and never skip.
    SMgpuCopyDecision decideMgpuCopy(bool haveDamage, bool damageEmpty, bool coversBuffer, uint32_t primed, size_t chainLength);

    struct SGlScissor {
        int  x    = 0;
        int  y    = 0;
        int  w    = 0;
        int  h    = 0;
        bool draw = false;
    };

    // Top-left box (x1,y1)-(x2,y2) into a bottom-left GL scissor, clipped to the buffer.
    // draw is false when the clipped box is empty, so the caller does not pass
    // a zero or negative rectangle to the driver.
    SGlScissor glScissorForTopLeftRect(int x1, int y1, int x2, int y2, int bufW, int bufH);
}
