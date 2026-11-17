---
name: complete-review
description: Full pre-merge review of a branch or PR in Antares Simulator. Runs mechanical gates (format, branch name, commit/PR conventions, build, tests, CI) then several independent review passes (flo-review, code-review, project guidelines, tests) and merges them into one verified report. Use for "complete review", "full review", or before marking a PR ready.
---

# Complete review (Antares Simulator)

Arguments: optional PR number or branch. Default: current branch against `develop`.
Never post to GitHub, push or edit code unless asked. Report only.

## 1. Scope

- `gh pr view <n> --json title,body,headRefName,headRefOid,baseRefName,isDraft,statusCheckRollup`
  or `git diff develop...HEAD --stat`.
- Get the diff once (`gh pr diff <n>` / `git diff`) and reuse it. Note the head SHA.

## 2. Mechanical gates (run them, do not eyeball)

Report each as PASS / FAIL with the evidence.

| Gate | How | Rule (source) |
|---|---|---|
| Branch name | compare `headRefName` | CI only runs for `feature/*`, `features/*`, `fix/*`, `release/*`, `issue-*`, `doc/*`, `tests/` (note `tests/`, not `test/`); `.github/agents/git-workflow.md`. CI job: `valid-branch-name` |
| Formatting | `clang-format --dry-run --Werror` on changed `.cpp/.h/.hpp` | CI requires clang-format 18.1.3. `src/format-code.sh` **exits with an error** on any other version: never discard its output (`>/dev/null`). Use `src/format-code-docker.sh` or run `clang-format` directly (18.1.8 gave identical results so far) |
| Commit messages | `git log develop..HEAD --format=%s` | conventional commits `type(scope): imperative`, subject under 50 chars (`.github/git-commit-instructions.md`) |
| PR description | read body | Motivation + Method, proportional to size; draft if not ready; docs updated if a documented feature changed (`docs/`) |
| Build | build only the affected targets in `~/CLionProjects/build_simulator` (`ninja <target>`) | working build dir, see memory |
| Tests | run the affected test binaries / `ctest -R` | a changed behaviour needs a test that fails without the change (mutate the fix and check) |
| CI | `gh pr checks <n>` | list failing checks and read the log of each before concluding |

## 3. Independent review passes

Run these as separate subagents in parallel (one message, several Agent calls). Give each only the
diff, the PR title/description and its own lens, so findings are not anchored on each other.
Each returns findings as `file:line | severity | claim | evidence`.

1. **Florian lens**: invoke `/flo-review <target>` instructions (correctness, tests, design, idiom, hygiene, docs/CI).
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
  confirm. See "Before reporting" in the flo-review skill.
- Rank: **HIGH** (wrong results, UB, weakened test, failing gate that blocks merge), **MED**, **NIT**.

## 5. Output

1. Verdict line: ready / ready after fixes / not ready, with the one-sentence reason.
2. Gate table (PASS/FAIL + evidence).
3. Findings, most severe first: `file:line — [severity] claim` with a suggested fix. Note which
   passes agreed.
4. Open questions for the author.

If asked to post: one GitHub review (`gh api repos/<o>/<r>/pulls/<n>/reviews`, `event: COMMENT`)
with inline comments on lines inside the diff, pinned to the head SHA.
