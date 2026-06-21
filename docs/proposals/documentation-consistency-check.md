# Proposal: Automated Documentation-Consistency Check on Pull Requests

| | |
|---|---|
| **Status** | Draft / for discussion |
| **Author** | (Antares maintainers) |
| **Date** | 2026-06-18 |
| **Scope** | CI tooling for `AntaresSimulatorTeam/Antares_Simulator` |

## 1. Problem

Antares ships two documentation surfaces:

- a **MkDocs** user/developer site under `docs/` (`mkdocs.yml`), and
- **Doxygen** API docs generated from the C++ sources (`docs/Doxyfile`, deployed by `.github/workflows/doxygen.yml`).

Today nothing in CI relates a code change to its documentation. A pull request can add or change a **user-facing surface** — a CLI flag, a study/config key, an input file format, a public API, or a behavior users rely on — and merge with the docs left stale. The drift is only caught later, by chance, during a manual read or a user bug report.

We want a **lightweight signal in the PR** that flags when a change has likely left the documentation incomplete or out of date, so the human reviewer can ask for a doc update before merge.

### Non-goals

- We cannot *guarantee* documentation is **correct** — no automated check can. The goal is a high-signal nudge, not a proof.
- We do not want to block trivial PRs (typo fixes, internal refactors, test-only changes).

## 2. Solution options

Four approaches were considered, from cheapest/most mechanical to most capable.

### Option 1 — "Touched code but not docs" reminder

A path-based rule (`dorny/paths-filter` or `actions/labeler`): if a PR changes `src/**` but nothing under `docs/**`, post a warning comment.

- **Pros:** trivial to implement, zero external dependency, free.
- **Cons:** very noisy — most `src/**` changes are internal and need no docs. Authors learn to ignore it. It detects *absence of a doc edit*, not *whether one was needed*.

### Option 2 — PR-template checkbox

Add `.github/PULL_REQUEST_TEMPLATE.md` with a `- [ ] Documentation updated (or N/A)` item, optionally enforced.

- **Pros:** cheap; puts judgment on the author.
- **Cons:** purely honor-system; easily checked without thought.

### Option 3 — Strict docs build as a required check

Make `mkdocs build --strict` and a Doxygen build with `WARN_AS_ERROR`/`WARN_IF_UNDOCUMENTED` **required** PR checks.

- **Pros:** catches *real* rot — broken links, missing nav entries, undocumented new public API. Deterministic, no external service.
- **Cons:** catches structural breakage only; it cannot tell that prose is semantically stale relative to a behavior change. Complementary to, not a substitute for, Option 4.

### Option 4 — AI/semantic consistency check (this proposal)

On each PR, send the **code diff** plus the **`docs/` file tree** (and any doc changes already in the PR) to an LLM, and ask one question: *given this code change, is the documentation now stale or incomplete, and where?* The model returns a structured verdict, posted as a single advisory PR comment.

> **Good idea!** To improve signal quality, the prompt should include contextual hints about which areas of the codebase are user-facing. For example: "format changes are found in `src/libs/antares`" — this helps the model focus on the right subsystems and reduces false positives from internal refactors in less visible areas.

- **Pros:** the only option that reasons about *semantic* consistency between code and prose. Catches "you added `--foo` but didn't document it" class of drift.
- **Cons:** probabilistic (false positives/negatives); depends on an external API and a secret; small per-PR cost.

### Recommendation

Adopt **Option 4 in advisory (comment-only) mode**, ideally paired later with **Option 3** as a hard gate for structural rot. Options 1 and 2 are low-value once Option 4 exists. The remainder of this document specifies Option 4.

## 3. Proposed design (Option 4)

### 3.1 Flow

```
pull_request (develop / main)
  └─ job: doc-consistency   [same-repo branches only]
       1. checkout base + head
       2. compute `git diff` restricted to src/** and docs/**
       3. one LLM call: diff + docs/ file tree → structured JSON verdict
       4. upsert a single sticky PR comment with the verdict
```

This is a **single LLM call** per PR (a classification/judgment task), not an autonomous agent crawling the repo. That keeps it cheap, fast, and predictable.

### 3.2 Structured verdict

The model is constrained to return a schema so the workflow can act on it deterministically:

```json
{
  "docs_update_needed": true,
  "confidence": "high",
  "findings": [
    {"area": "user-guide/solver/...", "reason": "New CLI flag --foo added in src/... but not documented"}
  ],
  "summary": "1 user-facing change appears undocumented."
}
```

The judge is instructed to flag **only user- or API-facing changes** (CLI flags, config/study keys, input/output formats, public APIs, user-visible behavior) and to ignore internal refactors, tests, and CI — biasing toward **precision over recall** so the comment stays trustworthy.

### 3.3 Output mode

- A **sticky advisory comment**, created once and updated in place on subsequent pushes.
- **Never a failing status check** to begin with. A probabilistic judge should inform the reviewer, not block a typo PR.
- Because the JSON already carries `docs_update_needed`, promoting it to a required check later is a one-line change once the team trusts the signal.

### 3.4 Implementation footprint

Two small files:

1. `.github/workflows/doc-consistency.yml` — workflow: checkout, compute diff, run the script, upsert the comment via the `gh` CLI. Matches existing CI conventions (Ubuntu 22.04, `actions/checkout@v6`, the `secrets.*` style already used in `sonarcloud.yml` and `doxygen.yml`).
2. `.github/scripts/doc_check.py` — ~50-line single-call script using the official `anthropic` Python SDK with schema-validated output.

### 3.5 Model

**`claude-haiku-4-5`** — this is a simple classification task ("is this change user-facing and undocumented?"), not a complex reasoning task. Haiku is faster, cheaper, and well-suited for this kind of structured judgment. (Higher-quality `claude-sonnet-4-6` or `claude-opus-4-8` are drop-in alternatives if precision proves insufficient.)

## 4. The fork-secrets constraint (key design decision)

Antares is a **public** repository. GitHub does **not** expose repository secrets (e.g. `ANTHROPIC_API_KEY`) to `pull_request` workflows triggered from **forked** repositories. This is the main practical constraint for any API-key-dependent check on an OSS project. Options:

| Approach | Secrets on fork PRs? | Security | Coverage |
|---|---|---|---|
| **Same-repo only** *(chosen)* | n/a (skips forks) | Highest — no untrusted code path | Misses external-contributor PRs |
| `pull_request_target` | Yes | Footgun if PR code is executed; contained here since we only read diff text | Full |
| Label-gated forks | Yes (after maintainer label) | Human-in-the-loop opt-in | Full, with friction |

**Chosen: same-repo only.** The workflow guards on `github.event.pull_request.head.repo.full_name == github.repository` and no-ops otherwise. This is the safest option and avoids the `pull_request_target` code-execution risk entirely. It also means the workflow can be merged **before** the secret exists — it simply does nothing until then.

The trade-off — external contributors' PRs are not checked — is acceptable for a first iteration and can be revisited (label-gating) if it proves limiting.

## 5. Cost

Per-PR cost is dominated by the input diff (typically a few thousand tokens) and a small JSON output (~500 tokens). Public per-million-token pricing:

| Model | Input $/1M | Output $/1M | Est. cost / PR* | Notes |
|---|---|---|---|---|
| `claude-opus-4-8` | $5.00 | $25.00 | ~$0.02–0.05 | Overkill for this task |
| `claude-sonnet-4-6` | $3.00 | $15.00 | ~$0.01–0.03 | Strong judgment, moderate cost |
| **`claude-haiku-4-5`** | **$1.00** | **$5.00** | **~$0.005–0.01** | **Chosen — fast, cheap, well-suited for classification** |

\* Assumes ~4k input + ~0.5k output tokens for a representative PR. Large diffs cost proportionally more; the script can cap/trim very large diffs.

**Order-of-magnitude:** at, say, 50 PRs/month on Haiku, this is roughly **$0.25–0.50/month** — negligible relative to the maintenance cost of stale docs. There is no fixed/idle cost; spend is strictly per-PR.

### Other costs

- **Engineering:** ~half a day to implement, plus a few weeks of comment-only observation to tune the prompt and confidence threshold.
- **External dependency:** one API key (Anthropic), stored as a GitHub repo secret. Single point of failure; if the key is unset or the API is down, the check no-ops (it must never block merges in advisory mode).
- **Privacy:** the PR diff (already public on a public repo) is sent to the model provider. No additional exposure for public PRs.

## 6. Risks & mitigations

| Risk | Mitigation |
|---|---|
| False positives erode trust | Bias prompt toward precision; advisory-only; sticky comment is easy to dismiss |
| False negatives (misses real drift) | Pair with Option 3 (strict docs build) for structural guarantees; treat as a net + over status quo |
| Secret/API outage | Same-repo-only + advisory mode → check no-ops, never blocks |
| Prompt drift as codebase evolves | Prompt lives in-repo (`doc_check.py`), versioned and reviewable |
| Cost surprise on huge PRs | Cap input diff size in the script |

## 7. Rollout plan

1. **Add the secret** — `ANTHROPIC_API_KEY` in repo Settings → Secrets (maintainer action; the workflow no-ops until present).
2. **Merge in comment-only mode** — observe verdicts on real PRs for ~2–4 weeks.
3. **Tune** the system prompt and confidence threshold based on observed precision.
4. **(Optional) Add Option 3** — strict MkDocs/Doxygen build as a required check for structural rot.
5. **(Optional) Promote** `docs_update_needed: true` from comment to a required status check once trusted.

## 8. Decision requested

- Approve adopting Option 4 in advisory mode with the same-repo-only / Haiku 4.5 configuration above?
- Approve provisioning an `ANTHROPIC_API_KEY` repository secret?
- Decide whether to bundle Option 3 (strict docs build) in the same effort.
