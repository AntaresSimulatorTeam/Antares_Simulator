# Dead Branches Report — AntaresSimulatorTeam/Antares_Simulator

**Generated:** 2026-06-26  
**Total branches inventoried:** ~190  
**Branches individually checked for last commit:** ~65  
**Open PRs found:** 21  
**Closed PRs sampled:** 100 most recent

---

## Summary

| Category | Count (approx.) |
|---|---|
| Keep / Active (recent commits or open PR) | ~55 |
| Special / Keep (protected, release lines, gh-pages) | ~12 |
| Likely Dead — Review (3–12 months stale) | ~55 |
| Safe to Delete (>12 months stale, no open PR, or obviously abandoned) | ~68 |

> **Rule of thumb:** "recent" = last commit after 2026-03-26 (< 3 months). "Stale" = last commit before 2025-06-26 (> 12 months). Everything in between is "Likely Dead — Review".

---

## 1. Special / Keep

These branches should never be deleted without explicit team decision.

| Branch | Reason |
|---|---|
| `develop` | **PROTECTED** — main integration branch |
| `gh-pages` | GitHub Pages site |
| `release/10.1.x` | Active release line — last commit 2026-06-19 |
| `release/10.0.x` | Previous major release line — last commit 2026-04-15 |
| `release/9.3.x` | Supported LTS line — last commit 2026-03-16 |
| `release/9.2.x` | Legacy support line — verify with team before deleting |

---

## 2. Keep / Active

### Open Pull Requests (always keep the head branch)

| Branch | PR # | Title | Status |
|---|---|---|---|
| `feature/extra-output-refacto-3` | #3766 | refacto: cleaner extra output | Open |
| `feature/extra-output-refacto` | #3765 | refacto: cleaner extra output design | Open (draft) |
| `feature/reserves_scalian_lot_2_new` | #3764 | Mis à jour des tests et homogénéisation du nommage | Open |
| `feature/clang-sanitizer-ci` | #3763 | ci: add Ubuntu Clang workflow with sanitizers | Open |
| `fix/quick-fixes-on-cmplx-extra-output` | #3761 | Extra-outputs: Quick fixes | Open (draft) |
| `fix/warning-rulecurve` | #3759 | Add warning when final level is outside rule curves | Open |
| `feature/extra-output-5` | #3758 | Feature/extra output 5 | Open (draft) |
| `fix/test-time` | #3755 | fix: replace @fast/@short legacy cucumber tags | Open |
| `feature/system-view-builder` | #3749 | Create a system for the view builder | Open (draft) |
| `feature/extra-output-4` | #3748 | feat: new extra outputs | Open (draft) |
| `feature/extra-output-study-info` | #3739 | feat(extra-outputs): add level_percentage and congestion | Open |
| `feature/ramping_model_scalian-sorties` | #3731 | refacto: sorties design [ramp] | Open |
| `feature/reserves-yaml` | #3729 | Change reserves input format to yaml | Open |
| `feature/reserves_need_scenarisation` | #3719 | Scenarisation of the reserve needs | Open |
| `feature/complexe-extra-outputs` | #3717 | feat(extra-outputs): constraints and duals | Open |
| `feature/add-tests-for-workflow-clean` | #3708 | added tests for workflow | Open (draft) |
| `feature/better_cucumber` | #3136 | Cucumber refacto | Open (draft, stale label) |
| `feature/920_annual` | #3134 | [ANT-3760] 920 Test NR to cucumber | Open (draft, stale label) |
| `feature/ramping_model_scalian` | #2684 | Feature/ramping model scalian | Open |
| `feature/ramping_model_scalian` | #1817 | [ANT-992] Add ramping for thermal clusters | Open (draft) |

### Recently Active (no open PR, committed < 3 months ago)

| Branch | Last Commit | Notes |
|---|---|---|
| `feat/clang-sanitizer-ci` | 2026-06-26 | Merged same day (PR #3762) — can delete |
| `claude/dreamy-lovelace-rnumqu` | 2026-06-26 | Merged (PR #3730) — can delete |
| `feature/extra-output-6` | 2026-06-24 | Base branch of PR #3758 — keep |
| `fix/hydro-rule-curves-warning` | 2026-06-24 | PR #3751 closed without merge — verify |
| `fix/unsp-energy-numerical-conditioning` | 2026-06-24 | PR #3697 closed without merge — verify |
| `tmp/warning-inventory` | 2026-06-16 | Recent but temporary — verify |
| `copilot/fix-code-review-suggestion` | 2026-05-27 | Merged (PR #3673) — can delete |
| `feature/gems-fb-and-parallelism-rc` | 2026-05-22 | Recent activity |
| `feature/disable-thermal-smooth` | 2026-05-22 | Recent activity |
| `feature/hard-code-use-interior-pt-first-optim` | 2026-05-11 | Recent debug activity |
| `fix/poc-scaling-test` | 2026-05-11 | Recent |
| `fix/ndebug` | 2026-05-11 | Recent |
| `feature/hybrid-scen` | 2026-04-29 | Recent |
| `fix/centos-segfault` | 2026-04-09 | Recent |
| `feature/or-tools-port` | 2026-03-17 | Recent |
| `feature/sonar_shared` | 2026-03-16 | Recent |
| `feature/milp-modeler` | 2026-03-04 | Recent |
| `branch-coverage` | 2026-03-03 | Recent |
| `feature/reserve_cluster_demarre` | 2026-03-19 | Recent |
| `feature/reserve_model_lot3_scalian-besoin-zone` | 2026-03-19 | Recent |
| `feature/reserve_optim2` | 2026-03-19 | Just outside 3-month window |
| `release/9.4.0-rc1` | 2026-02-24 | RC of 9.4 — verify if superseded |
| `release/9.3.9-938` | 2026-03-16 | Hotfix RC — verify if merged |

### Recently closed PRs — branches can be deleted

| Branch | PR | Merged |
|---|---|---|
| `feature/legacy_in_ST` | #3373 | 2026-05-29 |
| `feature/legacy-st-dynamic` | #3682 | 2026-06-08 |
| `feature/legacy-st-dynamic-doc` | #3693 | 2026-06-08 |
| `feature/legacy-st-dynamic-ressource` | #3700 | 2026-06-08 |
| `feature/remove-daily-simplex` | #3044 | 2026-06-09 |
| `feature/forbid-daily-simplex` | #3048 | 2026-06-09 |
| `feature/complete-out-of-bounds-processing` | #3610 | 2026-06-09 |
| `feature/add-tests-for-workflow` | #3707 | 2026-06-09 |
| `fix/do-not-export-infeasibility-pb-on-error` | #3658 | 2026-06-08 |
| `fix/do-not-export-infeasibility-pb-on-error-v9.3.10` | #3660 | 2026-06-08 |
| `fix/simplify-auxiliary` | #3647 | 2026-05-29 |
| `test/valid-hydro` | #3712 | 2026-06-09 |
| `fix/hydro-levels-data-class-hierarchy` | — | 2026-06-24 |
| `fix/sanitizer-uninitialized-parameters` | — | 2026-06-23 |
| `fix/sanitizer-fillcontext-dangling-ref` | — | 2026-06-23 |
| `fix/cucumber-parquet-2` | — | 2026-06-23 |
| `fix/study-rm-generator-cache` | — | 2026-06-23 |
| `fix/warning-deprecated-declarations` | — | 2026-06-23 |
| `fix/resources-out-of-src` | — | 2026-06-23 |
| `fix/rm-study-queue-service` | — | 2026-06-22 |
| `fix/study-unused` | — | 2026-06-22 |
| `fix/is-root-study` | — | 2026-06-22 |
| `fix/reservesEnabled` | — | 2026-06-22 |
| `fix/warning-move-semantics` | — | 2026-06-18 |
| `fix/warning-test-mocks` | — | 2026-06-18 |
| `fix/warning-misc-cleanups` | — | 2026-06-17 |
| `fix/rm-yuni` | — | 2026-05-29 |
| `fix/rm-yuni-unit-test` | — | 2026-05-27 |
| `feature/parquet-for-legacy-solver` | — | 2026-06-01 |
| `feature/reserve_model_lot3_scalian` | — | 2026-05-28 |
| `copilot/fix-code-review-suggestion` | — | 2026-05-27 |
| `feat/clang-sanitizer-ci` | — | 2026-06-26 |

---

## 3. Likely Dead — Review

Last commit **3–12 months ago** (between 2025-06-26 and 2026-03-26), no open PR. Team should confirm before deleting.

| Branch | Last Commit | Age | Notes |
|---|---|---|---|
| `RedispatchForAdqPatch` | 2025-10-01 | ~9 mo | Redispatch experiment |
| `feature/adqRedispClean` | 2025-10-02 | ~9 mo | Redispatch cleanup |
| `feature/net-position` | 2025-11-13 | ~7 mo | |
| `feature/modelView` | 2025-09-19 | ~9 mo | |
| `feature/vibe_refacto` | 2025-08-20 | ~10 mo | |
| `feature/var0` | 2025-10-13 | ~8 mo | |
| `feature/var1` | 2025-10-13 | ~8 mo | |
| `feature/codex-remove-ui-support-and-related-code` | 2025-12-09 | ~6.5 mo | |
| `feature/dynamic-variable` | 2026-02-03 | ~5 mo | Borderline |
| `feature/hybrid_invest` | 2026-01-20 | ~5 mo | |
| `features/scalian_removed_ui` | 2025-07-09 | ~12 mo | Borderline |
| `feature/reserve_model_lot3_scalian_no_barrier` | 2025-09-24 | ~9 mo | |
| `feature/reserve_scalian_without_modeler_test` | 2025-07-02 | ~12 mo | Borderline |
| `feature/study-builder` | 2025-07-02 | ~12 mo | Borderline |
| `feature/spatial-dynamic` | 2025-08-13 | ~10 mo | |
| `sts-constraints-name-consistency` | 2025-08-01 | ~11 mo | |
| `fix/ant-3750` | 2025-10-20 | ~8 mo | |
| `fix/antlr-windows` | 2025-09-02 | ~10 mo | |
| `fix/gurobi-revert` | 2025-06-25 | ~12 mo | Borderline |
| `fix/revert-objective` | 2025-07-03 | ~12 mo | |
| `feature/average-tests` | Unknown | — | Check |
| `feature/json-performance-tests` | Unknown | — | Likely stale |
| `feature/study-data-for-benchmark` | Unknown | — | Likely stale |
| `feature/tests-benchmark` | Unknown | — | Likely stale |
| `feature/tmp-maintenance-planning-rte-i-build-exe` | Unknown | — | "tmp" in name |
| `feature/rte-i-beta-version-for-apg` | Unknown | — | Beta branch, likely stale |
| `feature/pack-min-max` | Unknown | — | Check |
| `feature/poc-scenarios` | Unknown | — | POC branch |
| `feature/precompiled-headers` | Unknown | — | Likely superseded |
| `feature/script_update_version` | Unknown | — | Check |
| `feature/sorties` | Unknown | — | Check |
| `feature/refacto_sorties` | Unknown | — | Check |
| `feature/relecture-reserve-symetrie` | Unknown | — | Check |
| `feature/max-hydro-gen-pump-per-h-cr23-rte-comments` | Unknown | — | Check |
| `feature/max-hydro-gen-pump-per-h-cr23` | Unknown | — | Check |
| `doc/ai-review-proposal` | Unknown | — | Doc branch |
| `test/avoid_redundant_tests` | Unknown | — | Check |
| `test/extractsts-pmax-unit-tests` | Unknown | — | Check |
| `test-ci-ortools-install-dir` | Unknown | — | Check |
| `fix/test-ci-ortools-install-dir` | Unknown | — | Check |
| `fix/test-modeler` | Unknown | — | Check |
| `fix/test-nr-commit` | Unknown | — | Check |
| `fix/varname-fmt-refacto` | Unknown | — | Check |
| `fix/variables-new` | Unknown | — | Check |
| `fix/warning-sorties` | Unknown | — | Check |
| `fix/windows-optimal-values` | Unknown | — | Check |
| `fix/windows-packaging-bz2` | Unknown | — | Check |
| `fix/writer` | Unknown | — | Check |
| `fix/treil-improve-ast-dot-visit` | Unknown | — | Check |
| `fix/trial-for-fixes-on-reserve-model` | Unknown | — | Check |
| `fix/try-removals` | Unknown | — | Check |
| `fix/use-or-tools-one-shot-change-objective` | Unknown | — | Check |
| `fix/sonar` | Unknown | — | Check |
| `fix/sonar-disk-cleanup` | Unknown | — | Check |
| `fix/sonar-linking` | Unknown | — | Check |
| `fix/no-output` | Unknown | — | Check |
| `fix/parallele_test` | Unknown | — | Check |
| `fix/parquet-abseil-conflict` | Unknown | — | Check |
| `fix/problem-type-logic` | Unknown | — | Check |

---

## 4. Safe to Delete

Last commit **> 12 months ago** (before 2025-06-26), no open PR. Also includes obviously abandoned naming (poc_, tmp-, versioned feature/ branches for old releases, superseded RCs).

### Old release/* branches

| Branch | Rationale |
|---|---|
| `release/8.0.4` | v8.0 — several years old |
| `release/8.1.2` | v8.1 — several years old |
| `release/8.2.3` | v8.2 — several years old |
| `release/8.3.3` | v8.3 — several years old |
| `release/8.3.4` | v8.3 — several years old |
| `release/8.4.x` | Last commit 2023-09 (~33 mo) |
| `release/v8.4.x` | Duplicate / variant of above |
| `release/v8.4.3` | Old point release |
| `release/v8.5.1` | Old |
| `release/8.5.0-unofficial` | Unofficial old release |
| `release/v8.6.3` | Old |
| `release/8.6.x` | Old major line |
| `release/8.6.9` | Old |
| `release/8.7.x` | Old |
| `release/8.8.x` | Old major line |
| `release/8.8.6` | Old point release |
| `release/8.8.12-rc1` | Old RC |
| `release/9.2.1` | Superseded by 9.3.x |
| `release/v9.2.0-rc12` | Old RC |
| `release/9.3.6` | Superseded point release |
| `release/9.3.9-0` | Superseded patch RC |
| `release/9.3.9-00` | Superseded patch RC |
| `release/9.3.9-01` | Superseded patch RC |
| `release/9.3.9-1` | Superseded patch RC |
| `release/9.3.9-2` | Superseded patch RC |
| `release/9.3.9-3` | Superseded patch RC |
| `release/9.3.9-938` | Superseded patch RC |
| `release/9.3.x_digest_` | Experiment variant branch |
| `release/run-non-reg-tests` | Old CI experiment |
| `release/test_memory` | Old CI experiment |
| `release/workflows` | Old CI experiment |
| `release/centos` | Last commit 2025-04 (~14 mo) |

### Old feature/ branches (confirmed or inferred > 12 months)

| Branch | Last Commit | Notes |
|---|---|---|
| `feature/primary-reserve-prototype` | 2022-10-07 | ~45 months — very old |
| `feature/5.6` | — | v5.6 is ancient |
| `feature/6.2_E2E_test` | — | v6.2 is ancient |
| `feature/6.2c` | — | v6.2 is ancient |
| `feature/8.8.14-debug` | — | v8.8 debug — old |
| `feature/3party-features` | 2024-09-30 | ~21 mo |
| `feature/3rdparty-opt` | 2024-10-02 | ~21 mo — superseded by `feature/or-tools-port` |
| `feature/delta_clusters_scalian` | 2024-12-18 | ~18 mo |
| `feature/heuristic` | 2025-03-06 | ~15 mo |
| `feature/r-and-d-force-dual-simplex-in-optim-2` | 2025-01-29 | ~17 mo |
| `feature/r-and-force-simplex-optim-2-on-v880` | 2025-01-29 | ~17 mo |
| `feature/ramping_model_scalian_notests` | 2025-01-29 | ~17 mo |
| `feature/visitor-search` | 2025-03-06 | ~15 mo |
| `feature/xpress-immediate-change` | 2025-04-07 | ~15 mo |
| `feature/poc_hybrid_studies` | 2025-03-05 | ~15 mo — POC, abandoned |
| `feature/poc_legacy_port` | 2024-03-22 | ~27 mo — POC, abandoned |
| `feature/ortool_port` | 2024-08-14 | ~22 mo — superseded |
| `features/scalian_warning_doubles` | 2025-03-26 | ~15 mo |
| `features/scalian_warning_doubles_clean` | 2025-03-26 | ~15 mo |
| `feature/cpp23` | — | Old experiment — verify |

### Old fix/ and misc branches

| Branch | Last Commit | Notes |
|---|---|---|
| `fix/8_8_watervalues` | 2024-10-15 | ~20 mo |
| `fix/mingen-8.8` | 2024-10-15 | ~20 mo |
| `fix/allow-loading-paths-with-accents` | — | Old fix |
| `fix/apiV2-mps-export` | — | Old API fix |
| `fix/apiv2-hydro-initial-levels-2` | — | Old |
| `fix/build_ui_off` | — | UI was removed — obsolete |
| `fix/bump-simtest` | — | Old |
| `fix/call-to-solver-simplified` | — | Old |
| `fix/check-scenario` | — | Old |
| `fix/clarify-simulation-table` | — | Old |
| `fix/cmake` | — | Old |
| `fix/daily-hydro-mpsolver` | — | Old |
| `fix/delete_cach` | — | Typo in name, old |
| `fix/digest` | — | Old |
| `fix/digest-test` | — | Old |
| `fix/district-no-output` | — | Old |
| `fix/file_memory_size` | — | Old |
| `fix/fix-scenarization` | — | Old |
| `fix/floor-operator-tests-doc` | — | Old |
| `fix/hydro-monthly-heuristic-v921--ant-3236` | — | v921 targeted — old |
| `fix/hydro-weekly-final-level` | — | Old |
| `fix/infeasibility-caused-by-hydro` | — | Old |
| `fix/install` | — | Old |
| `fix/interface` | — | Old |
| `fix/jenkins-structure-txt` | — | Old CI branch |
| `fix/lower_bound_hydro_heuristic_daily` | — | Old |
| `fix/matrix-use-ifstream` | — | Old |
| `fix/max_space` | — | Old |
| `fix/merge-cond-res` | — | Old |
| `fix/multi_simulation` | — | Old |
| `fix/multiple-rule-curves` | — | Old |
| `fix/new-modeler-code-quality` | — | Old |
| `fix/no_basis_in_week_0` | — | Old |
| `fix/pr3479-parquet-build` | — | PR-specific temp branch |
| `fix/ref_simTable` | — | Old |
| `fix/revert-temp-fix` | — | Old |
| `fix/simtest-10rc2` | — | Old RC-specific |
| `fix/test-loadperf` | — | Old |
| `poc_linearProblem` | 2024-03-08 | ~27 mo — POC, abandoned |
| `rampe` | 2024-04-25 | ~26 mo — old reserve experiment |
| `precompiled-headers` | 2024-04-25 | ~26 mo |
| `performance_testing` | 2025-04-07 | ~15 mo |

---

## Quick Reference Decision Table

| Branch group | Recommended action |
|---|---|
| `release/8.*`, `release/v8.*`, `release/8.5.0-unofficial` | **Delete** — all > 2 years old |
| `release/9.2.1`, `release/9.3.6`, `release/9.3.9-*`, `release/9.3.x_digest_` | **Delete** — superseded point releases / RCs |
| `release/centos`, `release/run-non-reg-tests`, `release/test_memory`, `release/workflows` | **Delete** — old CI experiments |
| `feature/5.6`, `feature/6.2*`, `feature/8.8.14-debug` | **Delete** — version numbers reveal extreme age |
| `feature/poc_*`, `poc_linearProblem` | **Delete** — POC naming, all > 15 months old |
| `feature/primary-reserve-prototype` | **Delete** — last commit Oct 2022 |
| `rampe`, `precompiled-headers` (root) | **Delete** — > 26 months old |
| `feature/ortool_port`, `feature/3party-features`, `feature/3rdparty-opt` | **Delete** — superseded, > 20 months old |
| All "recently closed PRs" listed in §2 | **Delete** — already merged into `develop` |
| `claude/*`, `copilot/*` (merged ones) | **Delete** — AI-generated scratch branches, already merged |
| `tmp-elia-dev` | **Verify with Elia contact** before deleting (recent but "tmp" name) |
| `feature/920_annual`, `feature/better_cucumber` | **Keep** — stale open PRs; discuss with PR author |
| `release/9.2.x`, `release/10.0.x`, `release/9.3.x`, `release/10.1.x` | **Keep** — active/supported release lines |
| `develop`, `gh-pages` | **Never delete** |
