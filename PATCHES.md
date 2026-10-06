# Patches on `patched`

This branch tracks [hyprwm/aquamarine](https://github.com/hyprwm/aquamarine) `main` with a small set of personal fixes on top.

| Commit | Summary |
|--------|---------|
| `drm: guard async commit emit during teardown` | Upstream null-checks connectors in `flushAsyncCommitEvents`. This still skips empty connector SPs on VT switch, skips the commit signal when the connector is disconnected, and `lock()`s the backend in the idle callback so it does not run during teardown. |
| `drm: drop a cross-GPU blit instead of waiting on the other GPU's fence` | Explicit client fences are polled on the CPU. A fence that does not signal is a dropped frame, not an `eglWaitSync` on the scanout EGL display. Import of the other GPU's tiling is skipped. A page flip in flight for more than a second is dropped so later commits can proceed. The teardown guard above is unchanged. |
| `drm: render into the scanout buffer when the render GPU can` | Secondary outputs allocate their swapchain on the scanout GPU (`localScanout`, linear + scanout usage). If the render GPU can bind that buffer, KMS flips it and the copy is skipped. `AQ_MGPU_DIRECT_SCANOUT=0` keeps the blit. A failed import falls back to the blit and rebuilds the swapchain on the render GPU. |
| `drm: scissor multi-GPU blits and drop the CPU readback` | Remaining blits copy only the damage Hyprland committed, after one full frame per swapchain slot. An unchanged frame acquires the next scanout buffer and does not copy. A source the scanout GPU cannot import is a dropped frame, not a `glReadPixels` upload. |

## Updating from upstream

**Automated:** GitHub Actions rebases `patched` onto [hyprwm/aquamarine](https://github.com/hyprwm/aquamarine) `main` every Monday. If your patches conflict, the workflow fails and GitHub emails you (with default notification settings).

**Manual:**

```bash
git fetch upstream
git checkout patched
git rebase upstream/main
# fix conflicts if any, then:
git push --force-with-lease origin patched
```

Or from the PKGBUILD directory:

```bash
~/.local/share/pkgbuilds/aquamarine-patched/rebase-fork.sh
```

**Local rebuild** (by hand):

```bash
~/.local/share/pkgbuilds/aquamarine-patched/update.sh
```

The local `aquamarine-patched` PKGBUILD pulls this branch directly — no `.patch` files.

Rebuild **hyprland-patched after this package**. Hyprland links `libaquamarine.so`. Prefer:

```bash
sync-hypr-patched.sh
```
