# ADR: Management of Antares_Simulator_Tests_NR submodule version during releases

## Status

Accepted

## Date

2025-03-02

## Context

The repository uses a Git submodule at `src/tests/resources/Antares_Simulator_Tests_NR` which contains test data used by
CI workflows.
Historically some workflows used `git submodule update --init --remote` which follows the remote branch and therefore
may
pull a newer submodule head than the commit recorded in the superproject. This can lead to non-reproducible CI runs and
unexpected test differences between releases.

### Problem

If a release (for example v10.0) is created when the superproject records the submodule at commit `abc123`, but the
submodule remote later advances to `def456`, then running a workflow that uses `--remote` may fetch `def456` instead of
`abc123`. Re-running or re-building a release may therefore use different test inputs and yield different results.

## Decision

We adopt a clear, two-case policy for how the `Antares_Simulator_Tests_NR` submodule is initialized in CI workflows:

Policy
------

- Release builds (tagged releases, explicit release workflow runs): use the commit SHA recorded in the superproject
  (the pinned commit). This guarantees reproducibility for releases.
- Non-release builds (development branches such as `develop`, `feature/*`, CI runs outside release context): use the
  top of the `main` branch of the submodule (the latest tests on `origin/main`). This provides test teams quick access
  to the latest test data during development.

This policy strikes a balance: releases stay reproducible and traceable, while developers get fresh test data during
active development.

### How to detect a release in workflows

Workflows should consider a run to be a release when at least one of the following is true:

- the workflow is triggered on a tag (GITHUB_REF like `refs/tags/*`),
- the workflow event name is `release`,
- the workflow is called by the repository's explicit release workflow and receives an input `is_release: true`.

Workflows should treat other runs as development runs and checkout the top of `origin/main` in the submodule.

Implementation
--------------
Replace any use of `--remote` and enforce the two-case logic. Recommended pattern (bash) for a workflow step:

```bash
set -euo pipefail
SUBMODULE_PATH=src/tests/resources/Antares_Simulator_Tests_NR

# Initialize shallow by default (fast)
git submodule update --init --depth 1 -- "$SUBMODULE_PATH" || true

# Decision: release vs development
# prefer an explicit workflow input `is_release` when available
IS_RELEASE_INPUT=${{ inputs.is_release || 'false' }}
if [ "${IS_RELEASE_INPUT}" = "true" ] || [[ "$GITHUB_REF" == refs/tags/* ]] || [[ "$GITHUB_EVENT_NAME" == release ]]; then
  # Release: pin to the commit recorded in the superproject
  RECORDED_SHA=$(git ls-tree HEAD "$SUBMODULE_PATH" | awk '{print $3}')
  pushd "$SUBMODULE_PATH"
  if ! git rev-parse --verify "$RECORDED_SHA" >/dev/null 2>&1; then
    # If shallow clone doesn't have the object, fetch full history
    git fetch --unshallow || git fetch --all --tags --prune
  fi
  git checkout "$RECORDED_SHA"
  popd
else
  # Development: use the latest main from the submodule remote
  pushd "$SUBMODULE_PATH"
  git fetch origin main --depth=1 || git fetch --all --tags --prune
  if git show-ref --verify --quiet refs/remotes/origin/main; then
    git checkout origin/main
  elif git show-ref --verify --quiet refs/heads/main; then
    git checkout main
  elif git show-ref --verify --quiet refs/heads/master; then
    git checkout master
  fi
  popd
fi
```

Notes:

- When `is_release` is available as a workflow input (for reusable workflows invoked by the release orchestration), it
  should be honored.
- The shallow `--depth 1` clone is a performance optimization; if the pinned commit is missing from the shallow clone,
  a full fetch is attempted as a fallback.

## Consequences

### Positive

- Reproducibility for releases: release builds use the exact submodule commit recorded in the superproject.
- Fresh test data for development: developers and CI on non-release branches get the latest tests from `origin/main`.
- Traceability: release runs and artifacts can be traced back to exact submodule SHAs.

### Negative / trade-offs

- Developers get different test data between development and release contexts; this is intentional but should be
  documented so expectations are clear.
- If the submodule history is rewritten upstream and a recorded commit is removed, the release checkout will fail. The
  mitigation is to avoid force-pushes on the submodule's important branches and to pin releases to tags when possible.

## Procedure to update the submodule (maintainer action)

When you want a release (or a branch) to include a newer version of the tests, update the submodule HEAD in the
superproject and commit the change before tagging/releasing:

```bash
cd src/tests/resources/Antares_Simulator_Tests_NR
git fetch origin
git checkout origin/main   # or git checkout vX.Y.Z to pin a tag
cd ../../../..

# Record the new submodule state in the superproject
git add src/tests/resources/Antares_Simulator_Tests_NR
git commit -m "chore(tests): update Antares_Simulator_Tests_NR to <ref>"
```

For release preparation, ensure the superproject commit that you tag contains the expected submodule SHA.

## Alternatives considered

- Continue using `--remote`: rejected because it breaks reproducibility for releases.
- Always pin to the recorded commit (never use origin/main): rejected because developers lose easy access to the
  latest tests during development; the chosen two-case policy keeps development convenient.
- Publish test data as external artifact: valid long-term approach but requires infrastructure and convention changes.

## References

- Git Submodules Documentation: https://git-scm.com/book/en/v2/Git-Tools-Submodules
- Original issue: divergence of versions between consecutive releases
