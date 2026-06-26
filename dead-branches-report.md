# Dead Branches Report — AntaresSimulatorTeam/Antares_Simulator

**Generated:** 2026-06-26  
**Branches inventoried:** 170 (after cleanup pass)  
**Method:** `git log --no-merges -1` — merge commits from develop are excluded; owner = author of last real commit on the branch.

---

## Summary

| Category | Count |
|---|---|
| Special / Always Keep | 4 |
| Active (< 3 months, or open PR) | 57 |
| Likely Dead — Review (3–12 months) | 51 |
| Safe to Delete (> 12 months) | 58 |

> Cutoffs: **Active** = last real commit after 2026-03-26. **Safe to Delete** = before 2025-06-26. In between = Likely Dead.

---

## 1. Special / Always Keep

| Branch | Owner | Last Real Commit | Notes |
|---|---|---|---|
| `develop` | — | — | Protected — main integration branch |
| `gh-pages` | — | — | GitHub Pages |
| `release/10.1.x` | Jason Marechal | 2026-06-19 | Active release line |
| `release/9.3.x` | Jason Marechal | 2026-05-27 | LTS release line |

---

## 2. Active

Last real (non-merge) commit within the last 3 months, or has an open PR.

### With Open PR

| Branch | Owner | Last Real Commit | PR |
|---|---|---|---|
| `feature/extra-output-refacto-3` | Jason Marechal | 2026-06-26 | #3766 |
| `feature/reserves_scalian_lot_2_new` | h-fournier | 2026-06-26 | #3764 |
| `feature/complexe-extra-outputs` | Jason Marechal | 2026-06-26 | #3717 |
| `feature/extra-output-refacto` | Jason Marechal | 2026-06-26 | #3765 (draft) |
| `feature/extra-output-refacto-2` | Jason Marechal | 2026-06-26 | — |
| `feature/extra-output-study-info` | Jason Marechal | 2026-06-26 | #3739 |
| `fix/quick-fixes-on-cmplx-extra-output` | guilpierre | 2026-06-26 | #3761 (draft) |
| `feature/add-tests-for-workflow-clean` | LISASI Josue (Externe) | 2026-06-26 | #3708 (draft) |
| `feature/reserves-yaml` | payetvin | 2026-06-25 | #3729 |
| `fix/test-time` | Jason Maréchal | 2026-06-25 | #3755 |
| `feature/clang-sanitizer-ci` | Jason Maréchal | 2026-06-25 | #3763 |
| `fix/warning-rulecurve` | Florian OMNES | 2026-06-25 | #3759 |
| `feature/reserves_need_scenarisation` | h-fournier | 2026-06-24 | #3719 |
| `feature/extra-output-5` | Jason Marechal | 2026-06-24 | #3758 (draft) |
| `feature/extra-output-4` | Jason Marechal | 2026-06-23 | #3748 (draft) |
| `feature/ramping_model_scalian` | guilpier-code | 2026-06-23 | #2684, #1817 |
| `feature/system-view-builder` | payetvin | 2026-06-23 | #3749 (draft) |
| `feature/extra-output-6` | Jason Marechal | 2026-06-24 | (base of #3758) |
| `feature/ramping_model_scalian-sorties` | Jason Marechal | 2026-06-16 | #3731 |
| `feature/920_annual` | Jason Marechal | 2026-03-19 | #3134 (draft, stale label) |
| `feature/better_cucumber` | Jason Marechal | 2025-10-13 | #3136 (draft, stale label) |

### Recently Active (no open PR)

| Branch | Owner | Last Real Commit | Notes |
|---|---|---|---|
| `doc/ai-review-proposal` | Florian OMNES | 2026-06-21 | |
| `feat/extra-output-study-info` | Jason Marechal | 2026-06-22 | Check if superseded by `feature/` variant |
| `feature/disable-thermal-smooth` | Florian OMNES | 2026-06-16 | |
| `feature/ci-reserves` | payetvin | 2026-06-16 | |
| `fix/empty-optim-config-file` | Florian OMNES | 2026-06-08 | PR #3705 closed without merge |
| `fix/warning-sorties` | payetvin | 2026-06-09 | |
| `fix/unsp-energy-numerical-conditioning` | Thomas Bittar | 2026-06-03 | PR #3697 closed without merge |
| `claude/post-processing-csr-matching-p3ah2` | Claude | 2026-06-05 | AI scratch branch |
| `tmp/warning-inventory` | Claude | 2026-06-16 | AI scratch branch — delete when done |
| `tmp-elia-dev` | Florian OMNES | 2026-05-21 | Verify with Florian before deleting |
| `fix/ref_simTable` | Jason Maréchal | 2026-05-12 | |
| `fix/ndebug` | Florian OMNES | 2026-05-11 | |
| `feature/gems-fb-and-parallelism-rc` | Florian OMNES | 2026-05-22 | |
| `pr-3588` | Florian OMNES | 2026-04-23 | Scratch branch — can delete |
| `fix/merge-cond-res` | Florian OMNES | 2026-04-08 | |
| `fix/cmake` | GitHub Copilot | 2026-04-10 | |
| `fix/centos-segfault` | payetvin | 2026-04-09 | |
| `feature/hybrid-scen` | guilpier-code | 2026-04-29 | |
| `fix/digest-test` | Jason Marechal | 2026-03-31 | |
| `fix/district-no-output` | payetvin | 2026-03-30 | |
| `fix/digest` | Jason Marechal | 2026-03-27 | |
| `fix/try-removals` | guilpierre | 2026-03-24 | |
| `fix/hydro-rule-curves-warning` | Florian OMNES | 2026-06-23 | PR #3751 closed without merge |

### Borderline Active (just outside 3-month window — verify before deleting)

| Branch | Owner | Last Real Commit | Notes |
|---|---|---|---|
| `fix/simtest-10rc2` | payetvin | 2026-03-16 | |
| `feature/reserve_cluster_demarre` | sylvmara | 2026-03-16 | |
| `test-ci-ortools-install-dir` | OpenCode | 2026-03-16 | |
| `fix/test-ci-ortools-install-dir` | OpenCode | 2026-03-16 | |
| `feature/reserve_optim2` | sylvmara | 2026-03-19 | |
| `fix/sonar-disk-cleanup` | Florian Omnes | 2026-03-12 | |
| `fix/parquet-abseil-conflict` | OpenCode | 2026-03-13 | |
| `fix/pr3479-parquet-build` | Florian OMNES | 2026-03-12 | |
| `fix/multiple-rule-curves` | nikolaredstork | 2026-03-05 | |
| `feature/milp-modeler` | payetvin | 2026-03-04 | |
| `branch-coverage` | payetvin | 2026-03-03 | |
| `feature/reserve_model_lot3_scalian-besoin-zone` | sylvmara | 2026-02-26 | |
| `fix/parallele_test` | guilpier-code | 2026-02-12 | |
| `fix/windows-packaging-bz2` | Jason Marechal | 2026-02-23 | |

---

## 3. Likely Dead — Review

Last real commit **3–12 months ago** (2025-06-26 → 2026-03-26). No open PR. Confirm with the owner before deleting.

| Branch | Owner | Last Real Commit | Age |
|---|---|---|---|
| `fix/daily-hydro-mpsolver` | Florian OMNES | 2026-01-26 | ~5 mo |
| `fix/floor-operator-tests-doc` | Florian OMNES | 2026-01-18 | ~5.5 mo |
| `fix/problem-type-logic` | Abdoulbari Zakir | 2026-01-12 | ~5.5 mo |
| `fix/apiv2-hydro-initial-levels-2` | Abdoulbari Zakir | 2026-01-07 | ~5.5 mo |
| `fix/jenkins-structure-txt` | payetvin | 2026-01-07 | ~5.5 mo |
| `release/9.3.6` | Abdoulbari Zakir | 2026-01-21 | ~5 mo |
| `feature/average-tests` | payetvin | 2026-01-13 | ~5.5 mo |
| `fix/apiV2-mps-export` | dependabot[bot] | 2025-12-19 | ~6 mo |
| `feature/delete_cach` | Jason Marechal | 2025-11-03 | ~8 mo |
| `fix/delete_cach` | Jason Marechal | 2025-11-03 | ~8 mo |
| `fix/multi_simulation` | Florian Omnès | 2025-11-04 | ~8 mo |
| `fix/interface` | Florian OMNES | 2025-11-03 | ~8 mo |
| `feature/cpp23` | payetvin | 2026-02-16 | ~4 mo |
| `fix/max_space` | Jason Marechal | 2025-10-22 | ~8 mo |
| `fix/antlr-windows` | Vincent Payet | 2025-10-20 | ~8 mo |
| `fix/no-output` | Jason Maréchal | 2025-10-09 | ~9 mo |
| `fix/ant-3750` | Florian Omnès | 2025-09-02 | ~10 mo |
| `fix/fix-scenarization` | Florian OMNES | 2025-09-04 | ~10 mo |
| `fix/bump-simtest` | Florian OMNES | 2025-09-16 | ~9 mo |
| `fix/clarify-simulation-table` | guilpier | 2025-09-16 | ~9 mo |
| `fix/test-modeler` | Florian OMNES | 2025-09-16 | ~9 mo |
| `release/test_memory` | Jason Marechal | 2025-09-16 | ~9 mo |
| `fix/sonar-linking` | Florian OMNES | 2025-09-29 | ~9 mo |
| `fix/test-loadperf` | Florian OMNES | 2025-09-30 | ~9 mo |
| `feature/spatial-dynamic` | Vincent Payet | 2025-08-13 | ~10 mo |
| `fix/lower_bound_hydro_heuristic_daily` | Juliette-Gerbaux | 2025-08-29 | ~10 mo |
| `sts-constraints-name-consistency` | Sylvain Leclerc | 2025-08-01 | ~11 mo |
| `feature/study-builder` | Florian OMNES | 2025-07-02 | ~12 mo |
| `feature/remove-solver-headers` | Vincent Payet | 2025-07-02 | ~12 mo |
| `fix/matrix-use-ifstream` | Guillaume PIERRE | 2025-07-16 | ~11 mo |
| `fix/revert-objective` | Florian OMNES | 2025-07-03 | ~12 mo |
| `fix/revert-temp-fix` | Florian OMNES | 2025-07-24 | ~11 mo |
| `fix/no_basis_in_week_0` | Peter Mitri | 2025-07-24 | ~11 mo |
| `fix/test-nr-commit` | Vincent Payet | 2025-07-07 | ~12 mo |
| `fix/allow-loading-paths-with-accents` | Florian OMNES | 2025-07-11 | ~11.5 mo |
| `fix/windows-optimal-values` | Florian OMNES | 2025-06-30 | ~12 mo |
| `fix/gurobi-revert` | Florian OMNES | 2025-06-25 | ~12 mo |
| `release/9.2.x` | Jason Maréchal | 2025-08-07 | ~10 mo |
| `release/9.3.9-0` | Jason Marechal | 2026-03-19 | ~3 mo |
| `release/9.3.9-00` | payetvin | 2026-03-04 | ~3.7 mo |
| `release/9.3.9-01` | dependabot[bot] | 2026-03-09 | ~3.5 mo |
| `release/9.3.9-1` | Florian Omnès | 2026-03-31 | ~3 mo |
| `release/9.3.9-2` | Florian Omnès | 2026-03-31 | ~3 mo |
| `release/9.3.9-3` | Jason Maréchal | 2026-04-03 | ~3 mo |
| `release/9.3.x_digest_` | GitHub Copilot | 2026-05-13 | ~1.5 mo |
| `release/centos` | GitHub Copilot | 2026-04-08 | ~2.5 mo |
| `release/workflows` | Copilot | 2026-04-10 | ~2.5 mo |
| `release/8.8.x` | payetvin | 2026-04-23 | ~2 mo |
| `test/extractsts-pmax-unit-tests` | Florian OMNES | 2026-01-25 | ~5 mo |
| `fix/install` | Abdoulbari Zaher | 2026-02-06 | ~5 mo |
| `feature/hard-code-use-interior-pt-first-optim` | Thomas Bittar | 2025-05-07 | ~13.5 mo |

> Note: `release/9.3.9-*` are superseded patch RCs — safe to bulk-delete unless the final 9.3.9 tag hasn't been cut yet. `release/8.8.x` and `release/9.3.x_digest_` deserve a team discussion.

---

## 4. Safe to Delete

Last real commit **> 12 months ago** (before 2025-06-26). No open PR.

### Very old (> 2 years)

| Branch | Owner | Last Real Commit | Age |
|---|---|---|---|
| `feature/primary-reserve-prototype` | Hugo Kulesza | 2022-10-07 | ~45 mo |
| `feature/xpress-immediate-change` | Hugo Kulesza | 2022-08-04 | ~47 mo |
| `feature/json-performance-tests` | Florian OMNES | 2022-05-09 | ~49 mo |
| `release/run-non-reg-tests` | Guillaume PIERRE | 2022-02-22 | ~52 mo |
| `release/8.0.4` | Florian OMNES | 2022-03-18 | ~51 mo |
| `release/8.1.2` | Florian OMNES | 2022-03-21 | ~51 mo |
| `release/8.2.3` | Florian OMNES | 2022-12-05 | ~43 mo |
| `release/8.3.3` | Florian OMNES | 2022-12-16 | ~42 mo |
| `release/8.3.4` | Florian OMNES | 2023-09-01 | ~33 mo |
| `feature/max-hydro-gen-pump-per-h-cr23` | nikolaredstork | 2023-10-26 | ~32 mo |
| `feature/max-hydro-gen-pump-per-h-cr23-rte-comments` | NikolaIlic | 2023-10-26 | ~32 mo |
| `release/v8.4.3` | Jason Marechal | 2023-08-01 | ~35 mo |
| `release/v8.5.1` | Jason Marechal | 2023-08-01 | ~35 mo |
| `feature/rte-i-beta-version-for-apg` | Milos-RTEi | 2023-09-23 | ~33 mo |
| `release/v8.6.3` | Jason Marechal | 2023-10-04 | ~32 mo |
| `release/v8.4.x` | Vincent Payet | 2023-09-29 | ~33 mo |
| `fix/use-or-tools-one-shot-change-objective` | Florian OMNES | 2023-12-05 | ~31 mo |
| `fix/file_memory_size` | Abdoulbari ZAKIR | 2023-08-04 | ~35 mo |
| `fix/poc-scaling-test` | Florian OMNES | 2024-03-08 | ~27 mo |
| `poc_linearProblem` | MartinBelthle | 2024-03-08 | ~27 mo |
| `feature/poc-scenarios` | Florian OMNES | 2024-03-20 | ~27 mo |
| `feature/poc_legacy_port` | Peter Mitri | 2024-03-22 | ~27 mo |
| `release/8.5.0-unofficial` | Florian OMNES | 2024-03-19 | ~27 mo |
| `precompiled-headers` | Sylvain Leclerc | 2024-04-25 | ~26 mo |
| `rampe` | sylvmara | 2024-04-25 | ~26 mo |
| `feature/ortool_port` | Jason Marechal | 2024-08-14 | ~22 mo |
| `feature/3party-features` | Florian OMNES | 2024-09-30 | ~21 mo |
| `feature/3rdparty-opt` | Florian OMNES | 2024-10-02 | ~21 mo |
| `fix/call-to-solver-simplified` | Guillaume PIERRE | 2024-10-08 | ~21 mo |
| `fix/trial-for-fixes-on-reserve-model` | guilpier-code | 2024-10-03 | ~21 mo |
| `fix/mingen-8.8` | Juliette-Gerbaux | 2024-10-15 | ~20 mo |
| `feature/tmp-maintenance-planning-rte-i-build-exe` | Milos-RTEi | 2024-11-01 | ~20 mo |
| `release/8.6.x` | payetvin | 2024-06-28 | ~24 mo |
| `release/8.8.6` | Abdoulbari Zakir | 2024-06-28 | ~24 mo |
| `release/8.7.x` | payetvin | 2024-02-07 | ~28 mo |
| `release/8.8.12-rc1` | Florian OMNES | 2024-12-09 | ~18.5 mo |
| `feature/delta_clusters_scalian` | sylvmara | 2024-12-18 | ~18.5 mo |
| `feature/precompiled-headers` | Sylvain Leclerc | 2024-05-13 | ~25 mo |
| `release/8.6.9` | Vincent Payet | 2025-01-10 | ~17.5 mo |

### 12–15 months old

| Branch | Owner | Last Real Commit | Age |
|---|---|---|---|
| `feature/r-and-d-force-dual-simplex-in-optim-2` | Thomas Bittar | 2025-01-29 | ~17 mo |
| `feature/r-and-force-simplex-optim-2-on-v880` | Thomas Bittar | 2025-01-29 | ~17 mo |
| `feature/ramping_model_scalian_notests` | sylvmara | 2025-01-29 | ~17 mo |
| `fix/treil-improve-ast-dot-visit` | Guillaume PIERRE | 2025-03-21 | ~15 mo |
| `fix/new-modeler-code-quality` | Guillaume PIERRE | 2025-04-29 | ~14 mo |
| `feature/heuristic` | Florian OMNES | 2025-03-06 | ~15.5 mo |
| `feature/visitor-search` | Vincent Payet | 2025-03-06 | ~15.5 mo |
| `feature/poc_hybrid_studies` | Abdoulbari Zaher | 2025-03-05 | ~15.5 mo |
| `test/avoid_redundant_tests` | payetvin | 2025-03-04 | ~15.5 mo |
| `release/v9.2.0-rc12` | Florian OMNES | 2025-03-10 | ~15.5 mo |
| `features/scalian_warning_doubles` | sylvmara | 2025-03-26 | ~15 mo |
| `features/scalian_warning_doubles_clean` | sylvmara | 2025-03-26 | ~15 mo |
| `fix/8_8_watervalues` | Juliette-Gerbaux | 2025-04-24 | ~14 mo |
| `performance_testing` | Peter Mitri | 2025-04-07 | ~14.5 mo |
| `fix/build_ui_off` | Florian OMNES | 2025-05-02 | ~13.5 mo |
| `feature/hard-code-use-interior-pt-first-optim` | Thomas Bittar | 2025-05-07 | ~13.5 mo |
| `fix/hydro-monthly-heuristic-v921--ant-3236` | Guillaume PIERRE | 2025-06-11 | ~12.5 mo |
| `fix/infeasibility-caused-by-hydro` | Guillaume PIERRE | 2025-06-18 | ~12 mo |
| `fix/check-scenario` | Florian OMNES | 2025-06-20 | ~12 mo |
| `release/9.2.1` | Jason Marechal | 2025-04-11 | ~14.5 mo |
| `fix/hydro-weekly-final-level` | Juliette-Gerbaux | 2024-08-01 | ~23 mo |

---

## Quick Reference by Owner

| Owner | Safe to Delete | Needs Review (3–12 mo) |
|---|---|---|
| **Florian OMNES** | release/8.0.4–8.3.4, release/8.5.0-unofficial, release/v8.4–8.6.3, fix/use-or-tools*, fix/poc-scaling-test, fix/file_memory_size*, feature/3party-features, feature/3rdparty-opt, feature/json-performance-tests, feature/poc-scenarios, feature/heuristic, fix/build_ui_off, fix/check-scenario | fix/ant-3750, fix/bump-simtest, fix/clarify-simulation-table, fix/fix-scenarization, fix/gurobi-revert, fix/interface, fix/multi_simulation, fix/sonar-linking, fix/test-loadperf, fix/test-modeler, fix/revert-objective, fix/revert-temp-fix, fix/allow-loading-paths-with-accents, fix/windows-optimal-values, feature/study-builder |
| **Jason Marechal** | release/v8.4.3, release/v8.5.1, release/v8.6.3, feature/ortool_port, release/9.2.1 | fix/delete_cach, fix/max_space, fix/no-output, release/9.2.x, release/test_memory |
| **payetvin** | release/8.6.x–8.7.x, release/8.6.9, test/avoid_redundant_tests | fix/antlr-windows, fix/apiV2-mps-export, fix/jenkins-structure-txt, feature/average-tests, feature/cpp23, fix/test-nr-commit, feature/spatial-dynamic, release/9.3.9-00 |
| **sylvmara** | rampe, feature/ramping_model_scalian_notests, features/scalian_warning_doubles*, feature/delta_clusters_scalian | — |
| **Guillaume PIERRE** | release/run-non-reg-tests, fix/call-to-solver-simplified, fix/trial-for-fixes-on-reserve-model, fix/hydro-monthly-heuristic-v921*, fix/infeasibility-caused-by-hydro, fix/new-modeler-code-quality, fix/treil-improve-ast-dot-visit | fix/matrix-use-ifstream |
| **Thomas Bittar** | feature/r-and-d-force-dual-simplex-in-optim-2, feature/r-and-force-simplex-optim-2-on-v880, feature/hard-code-use-interior-pt-first-optim | — |
| **Juliette-Gerbaux** | fix/hydro-weekly-final-level, fix/mingen-8.8, fix/8_8_watervalues | fix/lower_bound_hydro_heuristic_daily |
| **Hugo Kulesza** | feature/primary-reserve-prototype (2022!), feature/xpress-immediate-change | — |
| **Milos-RTEi** | feature/rte-i-beta-version-for-apg, feature/tmp-maintenance-planning-rte-i-build-exe | — |
| **nikolaredstork** | feature/max-hydro-gen-pump-per-h-cr23* | — |
| **Sylvain Leclerc** | precompiled-headers, feature/precompiled-headers | sts-constraints-name-consistency |
| **Peter Mitri** | poc_linearProblem, feature/poc_legacy_port, performance_testing | fix/no_basis_in_week_0 |
| **Vincent Payet** | release/v8.4.x, release/8.6.9, feature/visitor-search | feature/spatial-dynamic, fix/test-nr-commit, feature/remove-solver-headers |
| **Abdoulbari Zakir** | fix/file_memory_size, release/8.8.6 | fix/apiv2-hydro-initial-levels-2, fix/problem-type-logic, release/9.3.6 |
| **MartinBelthle** | poc_linearProblem | — |
| **guilpier-code** | fix/trial-for-fixes-on-reserve-model | — |
