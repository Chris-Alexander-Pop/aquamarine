#include "MgpuCopy.hpp"
#include "shared.hpp"

using namespace Aquamarine;
using namespace Hyprutils::Math;

static int fullCopy(const SMgpuCopyDecision& decision) {
    return decision.full && !decision.skip;
}

static int skipped(const SMgpuCopyDecision& decision) {
    return decision.skip && decision.full;
}

static int partialCopy(const SMgpuCopyDecision& decision) {
    return !decision.full && !decision.skip;
}

int main(int, char**) {
    int ret = 0;

    const Vector2D panel = {3840, 2160};

    CRegion whole{0, 0, 3840, 2160};
    EXPECT(damageCoversBuffer(whole, panel), true);

    CRegion corner{10, 10, 40, 40};
    EXPECT(damageCoversBuffer(corner, panel), false);

    CRegion overhang{-8, -4, 4000, 2200};
    EXPECT(damageCoversBuffer(overhang, panel), true);

    CRegion shifted{1, 0, 3840, 2160};
    EXPECT(damageCoversBuffer(shifted, panel), false);

    // A bad buffer size must not turn into a scissor.
    EXPECT(damageCoversBuffer(corner, {0, 2160}), true);
    EXPECT(damageCoversBuffer(corner, {3840, -1}), true);

    CRegion empty;
    EXPECT(empty.empty(), true);
    damageCoversBuffer(empty, panel);

    // No damage flag, or no buffer: always a full copy, never a skip.
    EXPECT(fullCopy(decideMgpuCopy(false, true, false, 0, 3)), true);
    EXPECT(fullCopy(decideMgpuCopy(false, false, false, 9, 3)), true);
    EXPECT(fullCopy(decideMgpuCopy(false, true, true, 3, 3)), true);

    // Empty damage before the chain is primed still copies the whole frame.
    EXPECT(fullCopy(decideMgpuCopy(true, true, false, 0, 3)), true);
    EXPECT(fullCopy(decideMgpuCopy(true, true, false, 2, 3)), true);

    // Once every slot has a full image, an unchanged frame does not draw.
    EXPECT(skipped(decideMgpuCopy(true, true, false, 3, 3)), true);
    EXPECT(skipped(decideMgpuCopy(true, true, false, 4, 3)), true);

    // A zero-length chain is not primed. Do not skip onto it.
    EXPECT(fullCopy(decideMgpuCopy(true, true, false, 3, 0)), true);

    // Partial damage before the chain is primed is still a full copy.
    EXPECT(fullCopy(decideMgpuCopy(true, false, false, 0, 3)), true);
    EXPECT(fullCopy(decideMgpuCopy(true, false, false, 2, 3)), true);

    // Damage that covers the buffer stays a full copy even after priming.
    EXPECT(fullCopy(decideMgpuCopy(true, false, true, 3, 3)), true);

    // A chain that is not configured yet is a full copy, not a scissor.
    EXPECT(fullCopy(decideMgpuCopy(true, false, false, 3, 0)), true);

    // Primed chain, damage that does not cover: scissor.
    EXPECT(partialCopy(decideMgpuCopy(true, false, false, 3, 3)), true);
    EXPECT(partialCopy(decideMgpuCopy(true, false, false, 100, 3)), true);

    // Top 10px of a 100x50 buffer. GL y grows up, so this sits at y=40.
    const auto top = glScissorForTopLeftRect(0, 0, 10, 10, 100, 50);
    EXPECT(top.draw, true);
    EXPECT(top.x, 0);
    EXPECT(top.y, 40);
    EXPECT(top.w, 10);
    EXPECT(top.h, 10);

    // Bottom 10px sits on GL y=0.
    const auto bottom = glScissorForTopLeftRect(0, 40, 10, 50, 100, 50);
    EXPECT(bottom.draw, true);
    EXPECT(bottom.y, 0);
    EXPECT(bottom.h, 10);

    // A rect that hangs off every edge is clipped, and never goes negative.
    const auto hung = glScissorForTopLeftRect(-20, -10, 30, 400, 100, 50);
    EXPECT(hung.draw, true);
    EXPECT(hung.x, 0);
    EXPECT(hung.y, 0);
    EXPECT(hung.w, 30);
    EXPECT(hung.h, 50);
    EXPECT(hung.x >= 0 && hung.y >= 0, true);
    EXPECT(hung.x + hung.w <= 100 && hung.y + hung.h <= 50, true);

    EXPECT(glScissorForTopLeftRect(200, 200, 300, 300, 100, 50).draw, false);
    EXPECT(glScissorForTopLeftRect(10, 10, 5, 5, 100, 50).draw, false);
    EXPECT(glScissorForTopLeftRect(0, 0, 10, 10, 0, 50).draw, false);
    EXPECT(glScissorForTopLeftRect(0, 0, 10, 10, 100, 0).draw, false);
    EXPECT(glScissorForTopLeftRect(0, 0, 0, 10, 100, 50).draw, false);

    return ret;
}
