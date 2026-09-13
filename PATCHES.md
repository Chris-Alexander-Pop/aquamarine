# Patches on `patched`

This branch tracks [hyprwm/aquamarine](https://github.com/hyprwm/aquamarine) `main` with a small set of personal fixes on top.

| Commit | Summary |
|--------|---------|
| `drm: skip null connectors when flushing async commits` | `~CDRMBackend` resets connector SPs as it walks the vector, then `cancelAsyncOutput` flushes the whole list. `connector->output` on an empty SP is a NULL deref (Hyprland SIGSEGV in `flushAsyncCommitEvents` / `emitAsyncCommitEvent`). Idle commit callbacks now `lock()` the backend so they do not run during teardown. |

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
