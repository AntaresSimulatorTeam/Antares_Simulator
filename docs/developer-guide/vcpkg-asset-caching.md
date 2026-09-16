# vcpkg asset caching

Antares builds its dependencies with [vcpkg](https://github.com/microsoft/vcpkg). vcpkg has **two
independent caches**, and they solve different problems. This page describes both, but it is mostly
about the second one — the *asset* cache — and about the mirror that backs it, which lives in the
[`antares-vcpkg-registry`](https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry)
repository.

| | Binary cache | Asset cache |
|---|---|---|
| Environment variable | `VCPKG_BINARY_SOURCES` | `X_VCPKG_ASSET_SOURCES` |
| Caches | **compiled** packages (`.zip` per port + triplet + ABI hash) | **downloaded source archives** (tarballs, MSYS2 packages, vcpkg's own tools) |
| Saves | compilation time | network fetches, *and* survival of dead upstream URLs |
| Where it lives | `vcpkg_cache/` in the workspace, persisted by `actions/cache` | GitHub release assets on `antares-vcpkg-registry`, shared by all repos and all branches |
| Keyed by | ABI hash (compiler, triplet, port version, …) | SHA512 of the file |
| Lifetime | evicted by GitHub after 7 days of no use / 10 GB per repo | permanent, pruned by hand |

A binary cache miss costs a rebuild. An **asset** cache miss on a purged URL costs a *broken build
that no amount of retrying fixes* — which is why the asset mirror exists.

## The problem the asset mirror solves

Maintenance branches (`release/8.8.x`, `release/9.3.x`, `release/10.x`, …) pin an old vcpkg baseline
**on purpose**: bumping it would drag dependency upgrades and breaking changes into a branch whose
entire point is stability.

But a pinned port version points at an upstream download URL, and those URLs rot. MSYS2 in
particular deletes superseded packages within months. When that happens `vcpkg install` fails at the
*download* step, long before compiling anything: the source code is fine, the bytes are simply gone
from the internet. A branch that built last year no longer builds, and nothing in the branch itself
can be changed to fix it without breaking its whole reason to exist.

The asset cache is vcpkg's built-in answer: point it at a mirror holding a copy of every archive,
and downloads are served from there instead of from upstream.

```
                   hit
  vcpkg download ───────► antares-vcpkg-registry release  baseline-<sha>
        │                 (asset named <sha512 of the file>)
        │ miss
        └──────────────► upstream URL (github.com, msys2.org, …)  ← may 404
```

vcpkg verifies the SHA512 of every asset on every use, so a mirrored copy is either byte-identical
to what the port expects or it is rejected. **Provenance does not matter** — that property is what
makes the manual recovery procedure below legitimate.

## How this repository consumes the mirror

### The composite action

Every build workflow calls one composite action, [`.github/workflows/vcpkg-asset-cache/action.yml`](https://github.com/AntaresSimulatorTeam/Antares_Simulator/blob/develop/.github/workflows/vcpkg-asset-cache/action.yml),
right after bootstrapping vcpkg and before any `vcpkg install`:

```yaml
    - name: Configure vcpkg asset cache
      uses: ./.github/workflows/vcpkg-asset-cache
      with:
        base-url: ${{ vars.VCPKG_ASSET_BASE_URL }}
```

The action does two things:

1. **Reads the vcpkg baseline** — the `default-registry.baseline` field of `vcpkg-configuration.json`
   — and fails loudly if it cannot find a 40-hex value there. It is *read* rather than hardcoded
   because `develop`'s baseline moves every time the vcpkg submodule is bumped; on a release branch
   it is frozen, and the same code works unchanged.
2. **Exports `X_VCPKG_ASSET_SOURCES`** into `$GITHUB_ENV` for the rest of the job:

   ```
   clear;x-azurl,<base-url>/baseline-<baseline>/,,read
   ```

Reading the string field by field:

| Field | Value | Why |
|---|---|---|
| `clear` | — | drops every other asset source, including vcpkg's defaults, so the configuration is exactly what we wrote |
| `x-azurl` | the provider | despite the name it is a plain HTTP GET provider; any host that answers `GET <base>/<sha512>` works, GitHub releases included |
| URL | `…/releases/download/baseline-<sha>/` | **the trailing `/` is mandatory** — vcpkg appends the SHA512 directly to it |
| *(empty)* | SAS token | unused with GitHub releases; the field must still be present, hence the `,,` |
| `read` | access mode | GitHub releases answer `GET` but not `PUT`, so the mirror is read-only from CI; writes go through the seeding workflow |

!!! warning "Never add `x-block-origin` in CI"
    `x-block-origin` disables the fallback to the original URL. In a normal build that fallback is
    what lets still-alive URLs work when the mirror does not have them yet — and it is also what
    lets a *seeding* run discover new archives at all. Use `x-block-origin` only to deliberately
    prove that a file came from the mirror (see [Verifying](#verifying-the-mirror-actually-serves-a-file)).

### Why the base URL comes from a repository variable

Only the **base** URL is indirected, through the Actions variable `VCPKG_ASSET_BASE_URL`; the
baseline path segment is always computed from the checked-out tree. If the variable is unset the
action falls back to the public mirror URL, so nothing has to be configured for the workflows to
work — including on forks.

That split is deliberate. If the mirror ever has to move (to an Azure Blob container with a SAS
token, say, or to a private host), setting one organisation variable redirects every workflow on
every branch at once — **without re-touching frozen release branches**, which is exactly the churn
this mirror exists to avoid. The baseline, by contrast, is a property of the branch and must come
from the branch.

### Which workflows are wired up

`ubuntu.yml`, `windows-vcpkg.yml`, `oracle8.yml` and `sonarcloud.yml` all configure the asset cache
next to their existing binary cache restore. `new_release.yml` reuses those workflows, so releases
are covered too, and additionally *seeds* the mirror (see below).

### Using the mirror locally

Same string, computed by hand from your checkout:

```bash
export X_VCPKG_ASSET_SOURCES="clear;x-azurl,https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry/releases/download/baseline-$(jq -r '.["default-registry"].baseline' vcpkg-configuration.json)/,,read"
```

The mirror is public and read-only, so no credentials are involved. This is worth doing when
building an old maintenance branch locally: without it, the same purged-URL failure that hits CI
hits your machine.

## How the mirror itself works

### Layout

The mirror is **one GitHub release per vcpkg baseline**, tagged `baseline-<40-hex-sha>`, on
`AntaresSimulatorTeam/antares-vcpkg-registry`. Each release holds the source archives that baseline's
builds download, and **each asset is named exactly its SHA512** — 128 lowercase hex characters, no
extension. That naming is not a convention we chose; it is the layout the `x-azurl` provider looks
up.

The baseline is the natural partition key: it decides which port *versions* resolve, and therefore
which source URLs get downloaded. Because a branch's baseline is frozen, its release is a stable,
self-contained set that can be deleted outright when the branch is retired, with no effect on any
other branch. Archives shared between baselines are stored once per release; that duplication is the
price of independent prunability, and it is small — there are only a handful of live baselines.

The `baseline-` prefix is required, not decoration: a git ref whose name is bare SHA-1 hex is
ambiguous with the object id itself.

### Why that repository, and why GitHub releases

`antares-vcpkg-registry` has a second, unrelated job: it is the vcpkg *git registry* supplying
Antares-specific ports (`sirius-solver`, `or-tools-rte`, …), referenced from
`vcpkg-configuration.json`. The two uses do not interact — **release assets are not git objects**, so
they add nothing to the ~100 KB clone vcpkg performs when resolving the registry. Reusing it beats a
dedicated repository: it is already public org vcpkg infrastructure, already shared with
`Antares_Xpansion`, and creating a release adds only a tag, leaving the pinned registry baseline
untouched.

Release assets are free, unlimited in count, public without auth, served over a 302 redirect to a
signed blob URL that vcpkg's downloader follows, and need no infrastructure to maintain. The one
thing they cannot do is accept `PUT`, which is why the consumer configuration is `read`.

!!! note "The permission trade-off"
    GitHub has no releases-only permission, so anything that can seed the mirror (`Contents: write`)
    can also rewrite `ports/` and `versions/`. This is acceptable because consumers pin the registry
    by **baseline SHA** — a bad push to `main` cannot change what an existing branch resolves.
    `seed.yml` should stay the only workflow there holding `contents: write`.

### Seeding: cutting a release fills the mirror

The mirror is filled by [`seed.yml`](https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry/blob/main/.github/workflows/seed.yml)
in the registry repository. It builds a consumer repo at a given ref **with the asset cache
disabled** (`X_VCPKG_ASSET_SOURCES: clear`), so every archive is fetched from its origin, then
hashes everything that landed in `vcpkg/downloads/` and uploads what the mirror does not already
have, via `tools/upload-downloads.sh`.

It is a *reusable* workflow, and the intended trigger is a release. `new_release.yml` calls it once
per OS from a `seed_asset_mirror` job, gated on the `seed_asset_mirror` dispatch input (default
`true`):

```yaml
  seed_asset_mirror:
    name: Seed vcpkg asset mirror
    needs: release
    if: ${{ github.event.inputs.seed_asset_mirror == 'true' }}
    permissions:
      contents: write
    strategy:
      fail-fast: false
      matrix:
        include:
          - runner: ubuntu-22.04
            triplet: x64-linux
          - runner: windows-2022
            triplet: x64-windows-release
    uses: AntaresSimulatorTeam/antares-vcpkg-registry/.github/workflows/seed.yml@main
    with:
      repository: ${{ github.repository }}
      ref: ${{ needs.release.outputs.tag }}      # the tag, not the branch: pins the exact commit
      runner: ${{ matrix.runner }}
      triplet: ${{ matrix.triplet }}
    secrets:
      MIRROR_TOKEN: ${{ secrets.VCPKG_ASSET_PAT }}
```

A release is the right moment to capture: it is when a branch's baseline stops moving and becomes
something that has to stay buildable for years, and it is the point in `develop`'s history a
maintenance branch may later be cut from. Patch releases re-run it, so a branch still being
maintained keeps getting re-checked for free, and a newly cut branch is covered by its first release
rather than by someone remembering to register it somewhere.

Two GitHub-Actions subtleties are worth knowing before touching this job:

- **`MIRROR_TOKEN` is required.** This repository's `GITHUB_TOKEN` is scoped to this repository and
  cannot upload release assets to the registry. The organisation secret `VCPKG_ASSET_PAT` holds a
  token with `Contents: Read and write` on `antares-vcpkg-registry`. Same pattern as
  `SIMTEST_REPO_PAT` in the same workflow.
- **`permissions: contents: write` must be declared on the caller** even though the caller's token is
  not what performs the upload: GitHub validates a reusable workflow's requested permissions against
  the caller, whatever the event.

The run summary reports, per call, the baseline it resolved, how many assets were new, how many were
already mirrored, and — the part worth reading — **any archive whose origin already returns 404**.
Those are the ones only manual recovery can bring back.

### Seeding by hand

[`Seed asset mirror`](https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry/actions/workflows/seed.yml)
can also be dispatched directly on the registry repository, choosing the consumer repo, the ref, the
runner OS, the triplet and the mode. No token setup is involved on that path — the job's own
`GITHUB_TOKEN` already has the access. Use it:

- after a **baseline bump on `develop`**, to mirror the new baseline's archives while they are still
  alive;
- to **backfill** a branch released before this mechanism existed.

Two things to get right when dispatching:

- **Prioritise Windows.** That is where `vcpkg_acquire_msys` pulls the volatile MSYS2 packages, by far
  the most likely to disappear.
- `download-only` is much faster but best-effort — some ports only resolve their downloads during the
  build. If a Windows seed comes back without MSYS2 packages, re-run in `full-install` mode.

Seeding can only capture what **still downloads today**. Anything already purged upstream needs the
procedure below.

### Recovering an archive that is already gone upstream

This is the case that unblocks a broken maintenance branch. It works because vcpkg verifies SHA512 on
every use, so a file whose hash matches is by definition the right file, wherever it came from.

1. Read the expected hash straight out of the failing CI log — vcpkg prints it next to the dead URL:

   ```
   error: failed to download from mirror set
   Expected hash: 9f2c…   (128 hex chars)
   ```

2. Find the file anywhere: a developer machine with a warm `vcpkg/downloads/`, an old Docker image, a
   still-warm CI cache, a third-party archive mirror, a distro's source package.

3. Verify and upload:

   ```bash
   sha512sum thefile.tar.zst          # must match the expected hash exactly
   sha=$(sha512sum thefile.tar.zst | cut -d' ' -f1)
   cp thefile.tar.zst "$sha"
   gh release upload "baseline-<baseline-sha>" "$sha" \
       -R AntaresSimulatorTeam/antares-vcpkg-registry
   ```

   If the hash does not match, the file is the wrong one — do not upload it. vcpkg would reject it
   anyway.

### Bulk upload from a local downloads directory

If you have a machine that already built the branch successfully, its `vcpkg/downloads/` is a
complete seed:

```bash
tools/upload-downloads.sh /path/to/vcpkg/downloads <baseline-sha> --dry-run
tools/upload-downloads.sh /path/to/vcpkg/downloads <baseline-sha>
```

The script hashes every top-level file, skips what that baseline's release already holds, uploads the
rest, and creates the release if it does not exist. A bare 40-hex baseline is accepted and prefixed
automatically; `ASSETS_REPO` overrides the target repository.

## Verifying the mirror actually serves a file

Because origin fallback is on by default, a green build proves nothing about the mirror. To prove a
file really came from there, delete it locally and block the origin:

```bash
rm -f vcpkg/downloads/<the-file>
export X_VCPKG_ASSET_SOURCES="clear;x-azurl,https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry/releases/download/baseline-<sha>/,,read;x-block-origin"
./vcpkg/vcpkg install --x-manifest-root=. --triplet x64-linux-release
```

## Maintenance checklist

- **Bumped the vcpkg submodule / baseline on `develop`?** Dispatch `seed.yml` on the new baseline for
  Windows and Linux. Nothing breaks if you forget — origin fallback covers it — but the archives are
  only mirrored while they still exist.
- **Cutting a release?** Leave `seed_asset_mirror` at its default `true`.
- **Retiring a maintenance branch?** Its `baseline-<sha>` release can be deleted, once no other live
  branch pins the same baseline.
- **Moving the mirror?** Set the organisation variable `VCPKG_ASSET_BASE_URL`; do not edit release
  branches.
- **Adding a new build workflow?** Add the `Configure vcpkg asset cache` step after the vcpkg
  bootstrap and before any `vcpkg install`.

## References

- [vcpkg asset caching](https://learn.microsoft.com/en-us/vcpkg/users/assetcaching) — upstream documentation for `X_VCPKG_ASSET_SOURCES`
- [vcpkg binary caching](https://learn.microsoft.com/en-us/vcpkg/consume/binary-caching-local) — the other cache, `VCPKG_BINARY_SOURCES`
- [`antares-vcpkg-registry` README](https://github.com/AntaresSimulatorTeam/antares-vcpkg-registry#readme) — the mirror side, from the registry's point of view
- [Continuous Integration](continuous-integration.md) — the workflows this plugs into
