# Patches on `patched`

This branch tracks [hyprwm/aquamarine](https://github.com/hyprwm/aquamarine) `main` with a small set of personal fixes on top.

| Commit | Summary |
|--------|---------|
| `drm: guard async commit emit during teardown` | Upstream null-checks connectors in `flushAsyncCommitEvents`. This still skips empty connector SPs on VT switch, skips the commit signal when the connector is disconnected, and `lock()`s the backend in the idle callback so it does not run during teardown. |

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
