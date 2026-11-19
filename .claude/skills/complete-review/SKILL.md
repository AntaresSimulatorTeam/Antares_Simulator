---
name: complete-review
description: Full pre-merge review of a branch or PR in Antares Simulator. Runs mechanical gates (format, branch name, commit/PR conventions, build, tests, CI) then several independent review passes (maintainer checklist, bug hunt, project guidelines, tests) and merges them into one verified report. Use for "complete review", "full review", or before marking a PR ready.
---

# Complete review (Antares Simulator)

Arguments: optional PR number or branch. Default: current branch against `develop`.
Never post to GitHub, push or edit code unless asked. Report only.

## 1. Scope

- `gh pr view <n> --json title,body,headRefName,headRefOid,baseRefName,isDraft,statusCheckRollup`
  or `git diff develop...HEAD --stat`.
- Get the diff once (`gh pr diff <n>` for a PR; otherwise `git diff develop...<branch>`, with `<branch>` defaulting to `HEAD`) and reuse it. Note the reviewed head SHA.

## 2. Mechanical gates (run them, do not eyeball)

Report each as PASS / FAIL with the evidence.

| Gate | How | Rule (source) |
|---|---|---|
| Branch name | compare `headRefName` with `.github/workflows/branchNameValidation.yml` | Use the actual `valid-branch-name` rule; inspect each relevant workflow's triggers separately to determine whether CI runs (including `copilot/*`, `dependabot/**` and `tests/*` where configured). |
| Formatting | `clang-format --dry-run --Werror` on changed `.cpp/.h/.hpp` | CI requires clang-format 18.1.3. `src/format-code.sh` **exits with an error** on any other version: never discard its output (`>/dev/null`). Use `src/format-code-docker.sh` or run `clang-format` directly (18.1.8 gave identical results so far) |
| Commit messages | `git log develop..HEAD --format=%s` | conventional commits `type(scope): imperative`, subject under 50 chars (advisory; the enforced rule is the PR title below, which becomes the merge commit title, see `docs/developer-guide/6-Contributing.md`) |
| PR title and description | inspect title and read body | Title must match `^[a-z]+(\([^)]+\))?!?: .+ \[ANT-[0-9]+\]$` (Conventional Commit subject followed by an ANT JIRA key); keep it self-explanatory. Body: Motivation + Method, proportional to size; draft if not ready; docs updated if a documented feature changed (`docs/`) |
| Build | Configure a build directory from the reviewed source tree using the documented CMake options; build affected targets with `cmake --build <build-dir> --target <target>` | Use CMake's generator-independent build command; do not assume a particular path, generator, or session memory (`docs/developer-guide/3-Build.md`) |
| Tests | run the affected test binaries / `ctest -R` | a changed behaviour needs a test that fails without the change (mutate the fix and check) |
| CI | `gh pr checks <n>` | list failing checks and read the log of each before concluding |

## 3. Independent review passes

Run these as separate subagents in parallel (one message, several Agent calls). Give each only the
diff, the PR title/description and its own lens, so findings are not anchored on each other.
Each returns findings as `file:line | severity | claim | evidence`.

1. **Maintainer checklist**: apply `checklist.md` in this skill's directory (correctness, tests, design, idiom, hygiene, docs/CI).
2. **Bug hunt**: `/code-review high <target>` (correctness bugs only).
3. **Guidelines**: check the diff against `.github/agents/code-style.md`, `cpp-guidelines.md`,
   `error-handling.md`, `project-structure.md` (naming, includes, `const`, logging levels, exceptions).
4. **Tests and non-regression**: do tests cover every new branch and error path? Do they assert
   values, not just "no exception"? Does the change affect results (then a non-regression study in
   the tests repo is needed)? Are existing tests weakened?

A short run on a small diff may run the passes inline instead of spawning agents, but keep the
lenses separate.

## 4. Merge and verify

- Deduplicate findings reported by several passes (agreement raises confidence).
- Verify each remaining finding against the code on the PR head: quote the real line, and grep the
  repo for the convention before keeping a style NIT. Drop or mark "unverified" anything you cannot
  confirm. In particular:
  - re-read the cited line on the PR head (`git show <headRefOid>:<path>`); a finding must quote what
    the code does, not what the hunk suggests (e.g. "copies the element" is false if it is already `const auto&`);
  - grep the repo for the convention before keeping a style NIT (if sibling files do the same, drop it);
  - check every spelling variant of a pattern (`connexion_end` vs `connectionEnd`) so lists are complete;
  - inline comments can only target lines inside the PR diff hunks.
- Rank: **HIGH** (wrong results, UB, weakened test, failing gate that blocks merge), **MED**, **NIT**.

## 5. Output

1. Verdict line: ready / ready after fixes / not ready, with the one-sentence reason.
2. Gate table (PASS/FAIL + evidence).
3. Findings, most severe first: `file:line — [severity] claim` with a suggested fix. Note which
   passes agreed.
4. Open questions for the author.

If asked to post: one GitHub review (`gh api repos/<o>/<r>/pulls/<n>/reviews`, `event: COMMENT`)
with inline comments on lines inside the diff, pinned to the head SHA.
