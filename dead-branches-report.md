# Dead Branches Report — AntaresSimulatorTeam/Antares_Simulator

**Generated:** 2026-06-26  
**Total branches inventoried:** ~190  
**Open PRs found:** 21  
**Closed PRs sampled:** 100 most recent

---

## Summary

| Category | Count (approx.) |
|---|---|
| Keep / Active (recent commits or open PR) | ~55 |
| Special / Keep (protected, release lines, gh-pages) | ~6 |
| Likely Dead — Review (3–12 months stale) | ~55 |
| Safe to Delete (>12 months stale, no open PR, or obviously abandoned) | ~68 |

> **Rule of thumb:** "recent" = last commit after 2026-03-26 (< 3 months). "Stale" = last commit before 2025-06-26 (> 12 months). Everything in between is "Likely Dead — Review".

---

## 1. Special / Keep

These branches should never be deleted without explicit team decision.

| Branch | Owner | Reason |
|---|---|---|
| `develop` | — | **PROTECTED** — main integration branch |
| `gh-pages` | — | GitHub Pages site |
| `release/10.1.x` | JasonMarechal25 | Active release line — last commit 2026-06-19 |
| `release/10.0.x` | GitHub Copilot | Previous major release line — last commit 2026-04-15 |
| `release/9.3.x` | JasonMarechal25 | Supported LTS line — last commit 2026-03-16 |
| `release/9.2.x` | JasonMarechal25 | Legacy support line — verify with team before deleting |

---

## 2. Keep / Active

### Open Pull Requests (always keep the head branch)

| Branch | Owner | PR # | Title | Status |
|---|---|---|---|---|
| `feature/extra-output-refacto-3` | JasonMarechal25 | #3766 | refacto: cleaner extra output | Open |
| `feature/extra-output-refacto` | JasonMarechal25 | #3765 | refacto: cleaner extra output design | Open (draft) |
| `feature/reserves_scalian_lot_2_new` | h-fournier | #3764 | Mis à jour des tests et homogénéisation du nommage | Open |
| `feature/clang-sanitizer-ci` | JasonMarechal25 | #3763 | ci: add Ubuntu Clang workflow with sanitizers | Open |
| `fix/quick-fixes-on-cmplx-extra-output` | guilpierre | #3761 | Extra-outputs: Quick fixes | Open (draft) |
| `fix/warning-rulecurve` | flomnes | #3759 | Add warning when final level is outside rule curves | Open |
| `feature/extra-output-5` | JasonMarechal25 | #3758 | Feature/extra output 5 | Open (draft) |
| `fix/test-time` | JasonMarechal25 | #3755 | fix: replace @fast/@short legacy cucumber tags | Open |
| `feature/system-view-builder` | payetvin | #3749 | Create a system for the view builder | Open (draft) |
| `feature/extra-output-4` | JasonMarechal25 | #3748 | feat: new extra outputs | Open (draft) |
| `feature/extra-output-study-info` | JasonMarechal25 | #3739 | feat(extra-outputs): add level_percentage and congestion | Open |
| `feature/ramping_model_scalian-sorties` | JasonMarechal25 | #3731 | refacto: sorties design [ramp] | Open |
| `feature/reserves-yaml` | payetvin | #3729 | Change reserves input format to yaml | Open |
| `feature/reserves_need_scenarisation` | h-fournier | #3719 | Scenarisation of the reserve needs | Open |
| `feature/complexe-extra-outputs` | JasonMarechal25 | #3717 | feat(extra-outputs): constraints and duals | Open |
| `feature/add-tests-for-workflow-clean` | LISASI Josue (Externe) | #3708 | added tests for workflow | Open (draft) |
| `feature/better_cucumber` | JasonMarechal25 | #3136 | Cucumber refacto | Open (draft, stale label) |
| `feature/920_annual` | payetvin | #3134 | [ANT-3760] 920 Test NR to cucumber | Open (draft, stale label) |
| `feature/ramping_model_scalian` | JasonMarechal25 | #2684 | Feature/ramping model scalian | Open |
| `feature/ramping_model_scalian` | JasonMarechal25 | #1817 | [ANT-992] Add ramping for thermal clusters | Open (draft) |

### Recently Active (no open PR, committed < 3 months ago)

| Branch | Owner | Last Commit | Notes |
|---|---|---|---|
| `feature/extra-output-6` | JasonMarechal25 | 2026-06-24 | Base branch of PR #3758 — keep |
| `fix/hydro-rule-curves-warning` | guilpier-code | 2026-06-24 | PR #3751 closed without merge — verify |
| `fix/unsp-energy-numerical-conditioning` | tbittar | 2026-06-24 | PR #3697 closed without merge — verify |
| `tmp/warning-inventory` | claude | 2026-06-16 | Temporary — verify then delete |
| `feature/gems-fb-and-parallelism-rc` | flomnes | 2026-05-22 | Recent activity |
| `feature/disable-thermal-smooth` | flomnes | 2026-05-22 | Recent activity |
| `feature/hard-code-use-interior-pt-first-optim` | tbittar | 2026-05-11 | Recent debug activity |
| `fix/poc-scaling-test` | flomnes | 2026-05-11 | Recent |
| `fix/ndebug` | flomnes | 2026-05-11 | Recent |
| `feature/hybrid-scen` | JasonMarechal25 | 2026-04-29 | Recent |
| `fix/centos-segfault` | flomnes | 2026-04-09 | Recent |
| `feature/or-tools-port` | JasonMarechal25 | 2026-03-17 | Recent |
| `feature/sonar_shared` | JasonMarechal25 | 2026-03-16 | Recent |
| `feature/reserve_cluster_demarre` | sylvmara | 2026-03-19 | Recent |
| `feature/reserve_model_lot3_scalian-besoin-zone` | sylvmara | 2026-03-19 | Recent |
| `feature/reserve_optim2` | sylvmara | 2026-03-19 | Just outside 3-month window |
| `feature/milp-modeler` | payetvin | 2026-03-04 | Recent |
| `branch-coverage` | payetvin | 2026-03-03 | Recent |
| `release/9.4.0-rc1` | JasonMarechal25 | 2026-02-24 | RC of 9.4 — verify if superseded |
| `release/9.3.9-938` | JasonMarechal25 | 2026-03-16 | Hotfix RC — verify if merged |
| `claude/post-processing-csr-matching-p3ah2` | claude | — | AI-generated scratch — verify then delete |
| `copilot/available-ai-models` | JasonMarechal25 | — | Copilot-generated |
| `copilot/review-unit-test-application` | JasonMarechal25 | — | Copilot-generated |
| `feat/extra-output-study-info` | JasonMarechal25 | — | Check if superseded by feature/ variant |
| `feature/testNR` | JasonMarechal25 | 2026-03-02 | Recent |

### Recently closed/merged PRs — safe to delete

| Branch | Owner | Merged / Closed |
|---|---|---|
| `feature/legacy_in_ST` | *(deleted)* | 2026-05-29 |
| `feature/legacy-st-dynamic` | *(deleted)* | 2026-06-08 |
| `feature/legacy-st-dynamic-doc` | *(deleted)* | 2026-06-08 |
| `feature/legacy-st-dynamic-ressource` | *(deleted)* | 2026-06-08 |
| `feature/remove-daily-simplex` | *(deleted)* | 2026-06-09 |
| `feature/forbid-daily-simplex` | *(deleted)* | 2026-06-09 |
| `feature/complete-out-of-bounds-processing` | *(deleted)* | 2026-06-09 |
| `feature/add-tests-for-workflow` | *(deleted)* | 2026-06-09 |
| `fix/do-not-export-infeasibility-pb-on-error` | *(deleted)* | 2026-06-08 |
| `fix/do-not-export-infeasibility-pb-on-error-v9.3.10` | *(deleted)* | 2026-06-08 |
| `fix/simplify-auxiliary` | *(deleted)* | 2026-05-29 |
| `test/valid-hydro` | *(deleted)* | 2026-06-09 |
| `feat/clang-sanitizer-ci` | *(deleted)* | 2026-06-26 |
| `claude/dreamy-lovelace-rnumqu` | *(deleted)* | 2026-06-26 |
| `copilot/fix-code-review-suggestion` | *(deleted)* | 2026-05-27 |
| `fix/hydro-levels-data-class-hierarchy` | guilpier-code | 2026-06-24 |
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

> *(deleted)* = branch already removed from remote — no action needed.

---

## 3. Likely Dead — Review

Last commit **3–12 months ago**, no open PR. Confirm with the owner before deleting.

| Branch | Owner | Last Commit | Age | Notes |
|---|---|---|---|---|
| `RedispatchForAdqPatch` | aalzoobi | 2025-10-01 | ~9 mo | Redispatch experiment |
| `feature/adqRedispClean` | aalzoobi | 2025-10-02 | ~9 mo | Redispatch cleanup |
| `feature/net-position` | aalzoobi | 2025-11-13 | ~7 mo | |
| `feature/modelView` | a-zakir | 2025-09-19 | ~9 mo | |
| `feature/vibe_refacto` | JasonMarechal25 | 2025-08-20 | ~10 mo | |
| `feature/var0` | JasonMarechal25 | 2025-10-13 | ~8 mo | |
| `feature/var1` | JasonMarechal25 | 2025-10-13 | ~8 mo | |
| `feature/codex-remove-ui-support-and-related-code` | JasonMarechal25 | 2025-12-09 | ~6.5 mo | |
| `feature/dynamic-variable` | flomnes | 2026-02-03 | ~5 mo | Borderline |
| `feature/hybrid_invest` | JasonMarechal25 | 2026-01-20 | ~5 mo | |
| `features/scalian_removed_ui` | sylvmara | 2025-07-09 | ~12 mo | Borderline |
| `feature/reserve_model_lot3_scalian_no_barrier` | sylvmara | 2025-09-24 | ~9 mo | |
| `feature/reserve_scalian_without_modeler_test` | sylvmara | 2025-07-02 | ~12 mo | Borderline |
| `feature/study-builder` | flomnes | 2025-07-02 | ~12 mo | Borderline |
| `feature/spatial-dynamic` | payetvin | 2025-08-13 | ~10 mo | |
| `sts-constraints-name-consistency` | sylvlecl | 2025-08-01 | ~11 mo | |
| `fix/ant-3750` | flomnes | 2025-10-20 | ~8 mo | |
| `fix/antlr-windows` | guilpier-code | 2025-09-02 | ~10 mo | |
| `fix/gurobi-revert` | flomnes | 2025-06-25 | ~12 mo | Borderline |
| `fix/revert-objective` | flomnes | 2025-07-03 | ~12 mo | |
| `feature/average-tests` | payetvin | Unknown | — | Check |
| `feature/json-performance-tests` | flomnes | Unknown | — | Likely stale |
| `feature/study-data-for-benchmark` | a-zakir | Unknown | — | Likely stale |
| `feature/tests-benchmark` | a-zakir | Unknown | — | Likely stale |
| `feature/tmp-maintenance-planning-rte-i-build-exe` | Milos-RTEi | Unknown | — | "tmp" in name |
| `feature/rte-i-beta-version-for-apg` | Milos-RTEi | Unknown | — | Beta branch, likely stale |
| `feature/pack-min-max` | payetvin | Unknown | — | Check |
| `feature/poc-scenarios` | flomnes | Unknown | — | POC branch |
| `feature/precompiled-headers` | sylvlecl | Unknown | — | Likely superseded |
| `feature/script_update_version` | JasonMarechal25 | Unknown | — | Check |
| `feature/sorties` | JasonMarechal25 | Unknown | — | Check |
| `feature/refacto_sorties` | JasonMarechal25 | Unknown | — | Check |
| `feature/relecture-reserve-symetrie` | flomnes | Unknown | — | Check |
| `feature/max-hydro-gen-pump-per-h-cr23-rte-comments` | nikolaredstork | Unknown | — | Check |
| `feature/max-hydro-gen-pump-per-h-cr23` | nikolaredstork | Unknown | — | Check |
| `feature/remove-solver-headers` | payetvin | Unknown | — | Check |
| `feature/ci-reserves` | payetvin | Unknown | — | Check |
| `feature/coverage` | JasonMarechal25 | Unknown | — | Check |
| `doc/ai-review-proposal` | flomnes | Unknown | — | Doc branch |
| `test/avoid_redundant_tests` | pet-mit | Unknown | — | Check |
| `test/extractsts-pmax-unit-tests` | flomnes | Unknown | — | Check |
| `test-ci-ortools-install-dir` | OpenCode | Unknown | — | Check |
| `fix/test-ci-ortools-install-dir` | OpenCode | Unknown | — | Check |
| `fix/test-modeler` | flomnes | Unknown | — | Check |
| `fix/test-nr-commit` | payetvin | Unknown | — | Check |
| `fix/varname-fmt-refacto` | JasonMarechal25 | Unknown | — | Check |
| `fix/variables-new` | JasonMarechal25 | Unknown | — | Check |
| `fix/warning-sorties` | payetvin | Unknown | — | Check |
| `fix/windows-optimal-values` | flomnes | Unknown | — | Check |
| `fix/windows-packaging-bz2` | payetvin | Unknown | — | Check |
| `fix/writer` | JasonMarechal25 | Unknown | — | Check |
| `fix/treil-improve-ast-dot-visit` | guilpier-code | Unknown | — | Check |
| `fix/trial-for-fixes-on-reserve-model` | guilpier-code | Unknown | — | Check |
| `fix/try-removals` | guilpierre | Unknown | — | Check |
| `fix/use-or-tools-one-shot-change-objective` | flomnes | Unknown | — | Check |
| `fix/sonar` | JasonMarechal25 | Unknown | — | Check |
| `fix/sonar-disk-cleanup` | flomnes | Unknown | — | Check |
| `fix/sonar-linking` | flomnes | Unknown | — | Check |
| `fix/no-output` | flomnes | Unknown | — | Check |
| `fix/parallele_test` | guilpier-code | Unknown | — | Check |
| `fix/parquet-abseil-conflict` | OpenCode | Unknown | — | Check |
| `fix/problem-type-logic` | flomnes | Unknown | — | Check |
| `pr-3588` | flomnes | 2026-04-23 | ~2 mo | Scratch branch — can delete |

---

## 4. Safe to Delete

Last commit **> 12 months ago**, no open PR. Also includes obviously abandoned branches (POC, versioned feature/ for old releases, superseded RCs).

### Old release/* branches

| Branch | Owner | Rationale |
|---|---|---|
| `release/8.0.4` | flomnes | v8.0 — several years old |
| `release/8.1.2` | flomnes | v8.1 — several years old |
| `release/8.2.3` | flomnes | v8.2 — several years old |
| `release/8.3.3` | flomnes | v8.3 — several years old |
| `release/8.3.4` | flomnes | v8.3 — several years old |
| `release/8.4.x` | — | Last commit 2023-09 (~33 mo) |
| `release/v8.4.x` | flomnes | Duplicate / variant of above |
| `release/v8.4.3` | flomnes | Old point release |
| `release/v8.5.1` | flomnes | Old |
| `release/8.5.0-unofficial` | flomnes | Unofficial old release |
| `release/v8.6.3` | JasonMarechal25 | Old |
| `release/8.6.x` | payetvin | Old major line |
| `release/8.6.9` | payetvin | Old |
| `release/8.7.x` | payetvin | Old |
| `release/8.8.x` | payetvin | Old major line |
| `release/8.8.6` | a-zakir | Old point release |
| `release/8.8.12-rc1` | flomnes | Old RC |
| `release/9.2.1` | JasonMarechal25 | Superseded by 9.3.x |
| `release/v9.2.0-rc12` | flomnes | Old RC |
| `release/9.3.6` | payetvin | Superseded point release |
| `release/9.3.9-0` | JasonMarechal25 | Superseded patch RC |
| `release/9.3.9-00` | dependabot[bot] | Superseded patch RC |
| `release/9.3.9-01` | dependabot[bot] | Superseded patch RC |
| `release/9.3.9-1` | JasonMarechal25 | Superseded patch RC |
| `release/9.3.9-2` | flomnes | Superseded patch RC |
| `release/9.3.9-3` | JasonMarechal25 | Superseded patch RC |
| `release/9.3.x_digest_` | a-zakir | Experiment variant branch |
| `release/run-non-reg-tests` | GitHub Copilot | Old CI experiment |
| `release/test_memory` | guilpier-code | Old CI experiment |
| `release/workflows` | Copilot | Old CI experiment |
| `release/centos` | Juliette-Gerbaux | Last commit 2025-04 (~14 mo) |

### Old feature/ branches (confirmed or inferred > 12 months)

| Branch | Owner | Last Commit | Notes |
|---|---|---|---|
| `feature/primary-reserve-prototype` | HugoKulesza | 2022-10-07 | ~45 months — very old |
| `feature/5.6` | JasonMarechal25 | — | v5.6 is ancient |
| `feature/6.2_E2E_test` | JasonMarechal25 | — | v6.2 is ancient |
| `feature/6.2c` | JasonMarechal25 | — | v6.2 is ancient |
| `feature/8.8.14-debug` | JasonMarechal25 | — | v8.8 debug — old |
| `feature/3party-features` | flomnes | 2024-09-30 | ~21 mo |
| `feature/3rdparty-opt` | flomnes | 2024-10-02 | ~21 mo — superseded by `feature/or-tools-port` |
| `feature/delta_clusters_scalian` | sylvmara | 2024-12-18 | ~18 mo |
| `feature/heuristic` | flomnes | 2025-03-06 | ~15 mo |
| `feature/r-and-d-force-dual-simplex-in-optim-2` | tbittar | 2025-01-29 | ~17 mo |
| `feature/r-and-force-simplex-optim-2-on-v880` | tbittar | 2025-01-29 | ~17 mo |
| `feature/ramping_model_scalian_notests` | sylvmara | 2025-01-29 | ~17 mo |
| `feature/visitor-search` | payetvin | 2025-03-06 | ~15 mo |
| `feature/xpress-immediate-change` | HugoKulesza | 2025-04-07 | ~15 mo |
| `feature/poc_hybrid_studies` | pet-mit | 2025-03-05 | ~15 mo — POC, abandoned |
| `feature/poc_legacy_port` | pet-mit | 2024-03-22 | ~27 mo — POC, abandoned |
| `feature/ortool_port` | JasonMarechal25 | 2024-08-14 | ~22 mo — superseded |
| `features/scalian_warning_doubles` | sylvmara | 2025-03-26 | ~15 mo |
| `features/scalian_warning_doubles_clean` | sylvmara | 2025-03-26 | ~15 mo |
| `feature/cpp23` | JasonMarechal25 | — | Old experiment |

### Old fix/ and misc branches

| Branch | Owner | Last Commit | Notes |
|---|---|---|---|
| `fix/8_8_watervalues` | JasonMarechal25 | 2024-10-15 | ~20 mo |
| `fix/mingen-8.8` | JasonMarechal25 | 2024-10-15 | ~20 mo |
| `fix/allow-loading-paths-with-accents` | flomnes | — | Old fix |
| `fix/apiV2-mps-export` | payetvin | — | Old API fix |
| `fix/apiv2-hydro-initial-levels-2` | a-zakir | — | Old |
| `fix/build_ui_off` | flomnes | — | UI was removed — obsolete |
| `fix/bump-simtest` | flomnes | — | Old |
| `fix/call-to-solver-simplified` | guilpier-code | — | Old |
| `fix/check-scenario` | JasonMarechal25 | — | Old |
| `fix/clarify-simulation-table` | guilpierre | — | Old |
| `fix/cmake` | GitHub Copilot | — | Old |
| `fix/daily-hydro-mpsolver` | flomnes | — | Old |
| `fix/delete_cach` | flomnes | — | Typo in name, old |
| `fix/digest` | flomnes | — | Old |
| `fix/digest-test` | payetvin | — | Old |
| `fix/district-no-output` | JasonMarechal25 | — | Old |
| `fix/file_memory_size` | a-zakir | — | Old |
| `fix/fix-scenarization` | flomnes | — | Old |
| `fix/floor-operator-tests-doc` | flomnes | — | Old |
| `fix/hydro-monthly-heuristic-v921--ant-3236` | guilpier-code | — | v921 targeted — old |
| `fix/hydro-weekly-final-level` | Juliette-Gerbaux | — | Old |
| `fix/infeasibility-caused-by-hydro` | guilpier-code | — | Old |
| `fix/install` | JasonMarechal25 | — | Old |
| `fix/interface` | flomnes | — | Old |
| `fix/jenkins-structure-txt` | payetvin | — | Old CI branch |
| `fix/lower_bound_hydro_heuristic_daily` | Juliette-Gerbaux | — | Old |
| `fix/matrix-use-ifstream` | guilpier-code | — | Old |
| `fix/max_space` | JasonMarechal25 | — | Old |
| `fix/merge-cond-res` | flomnes | — | Old |
| `fix/multi_simulation` | JasonMarechal25 | — | Old |
| `fix/multiple-rule-curves` | nikolaredstork | — | Old |
| `fix/new-modeler-code-quality` | guilpier-code | — | Old |
| `fix/no_basis_in_week_0` | pet-mit | — | Old |
| `fix/pr3479-parquet-build` | OpenCode | — | PR-specific temp branch |
| `fix/ref_simTable` | JasonMarechal25 | — | Old |
| `fix/revert-temp-fix` | flomnes | — | Old |
| `fix/simtest-10rc2` | payetvin | — | Old RC-specific |
| `fix/test-loadperf` | flomnes | — | Old |
| `poc_linearProblem` | MartinBelthle | 2024-03-08 | ~27 mo — POC, abandoned |
| `rampe` | sylvmara | 2024-04-25 | ~26 mo — old reserve experiment |
| `precompiled-headers` | sylvlecl | 2024-04-25 | ~26 mo |
| `performance_testing` | pet-mit | 2025-04-07 | ~15 mo |

---

## Quick Reference by Owner

| Owner | Branches to review / delete |
|---|---|
| **JasonMarechal25** | feature/var0, feature/var1, feature/vibe_refacto, feature/codex-remove-ui-support-and-related-code, feature/hybrid_invest, feature/5.6, feature/6.2*, feature/8.8.14-debug, feature/ortool_port, fix/8_8_watervalues, fix/mingen-8.8, release/v8.6.3, release/9.2.1, release/9.3.9-* (most) |
| **flomnes** | feature/3party-features, feature/3rdparty-opt, feature/heuristic, feature/poc-scenarios, feature/study-builder, feature/dynamic-variable, fix/ant-3750, fix/gurobi-revert, fix/revert-objective, release/v8.4.x, release/8.0.4–8.3.x, release/v8.5.1 |
| **sylvmara** | feature/delta_clusters_scalian, feature/ramping_model_scalian_notests, feature/reserve_model_lot3_scalian_no_barrier, features/scalian_*, rampe |
| **payetvin** | feature/average-tests, feature/spatial-dynamic, feature/visitor-search, fix/apiV2-mps-export, fix/warning-sorties, release/8.6.x–8.8.x |
| **sylvlecl** | sts-constraints-name-consistency, feature/precompiled-headers, precompiled-headers |
| **tbittar** | feature/r-and-d-force-dual-simplex-in-optim-2, feature/r-and-force-simplex-optim-2-on-v880, fix/unsp-energy-numerical-conditioning |
| **guilpier-code** | fix/antlr-windows, fix/call-to-solver-simplified, fix/hydro-monthly-heuristic-v921--ant-3236, fix/infeasibility-caused-by-hydro, fix/new-modeler-code-quality, fix/parallele_test, fix/trial-for-fixes-on-reserve-model |
| **pet-mit** | feature/poc_hybrid_studies, feature/poc_legacy_port, fix/no_basis_in_week_0, poc_linearProblem, performance_testing, test/avoid_redundant_tests |
| **HugoKulesza** | feature/primary-reserve-prototype (2022!), feature/xpress-immediate-change |
| **nikolaredstork** | feature/max-hydro-gen-pump-per-h-cr23*, fix/multiple-rule-curves |
| **a-zakir** | feature/modelView, feature/study-data-for-benchmark, feature/tests-benchmark, fix/apiv2-hydro-initial-levels-2, fix/file_memory_size |
| **Milos-RTEi** | feature/rte-i-beta-version-for-apg, feature/tmp-maintenance-planning-rte-i-build-exe |
| **aalzoobi** | RedispatchForAdqPatch, feature/adqRedispClean, feature/net-position |
| **Juliette-Gerbaux** | fix/hydro-weekly-final-level, fix/lower_bound_hydro_heuristic_daily, release/centos |
| **MartinBelthle** | poc_linearProblem |
| **OpenCode** | fix/parquet-abseil-conflict, fix/pr3479-parquet-build, test-ci-ortools-install-dir |
| **h-fournier** | *(all branches have open PRs — keep)* |
