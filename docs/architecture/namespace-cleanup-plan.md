# Namespace Cleanup Plan

Author: assessment for a Sonnet implementation pass.
Build check after every step: `cmake --build --target all --preset Debug-vcpkg`
(binary: `/home/jmarechal/miniconda3/bin/cmake`).

## Goal

The `Antares` namespace tree has drifted. This plan fixes three concrete
problems, in priority order:

1. **`Optimisation` vs `Optimization`** — two spellings coexist and mean
   different things, which is genuinely misleading.
2. **The `src/solver/optimisation/` directory mixes four namespaces.**
3. **Global-namespace leftovers** — French-named structs and un-scoped
   headers.

Everything below is scoped so each item can be done and built independently.
Do them in order; stop and rebuild between items.

---

## Assessment of the current state

### The two optim namespaces

| Namespace | Spelling | Meaning today | Location |
|-----------|----------|---------------|----------|
| `Antares::Optimisation::*` | British | **New** modeler linear-problem framework (`LinearProblemApi`, `LinearProblemDataImpl`, `LinearProblemMpsolverImpl`) | `src/optimisation/`, `src/modeler-optimisation-container/`, and 3 leaked files in `src/solver/optimisation/` |
| `Antares::Optimization` | American | **Legacy** solver optimization (`Legacy*` fillers, options, infeasible-problem-analysis, optim-model-filler, basis_status) | `src/solver/optimisation/`, `src/solver/optim-model-filler/`, `src/libs/antares/optimization-options/`, `src/solver/infeasible-problem-analysis/`, `src/solver/utils/` |
| `Antares::Solver::Optimization` | American | A *third* namespace — weekly optimization + CSR quadratic problem | `src/solver/optimisation/weekly_optimization.h`, `.../adequacy_patch_csr/` |

So "Optimisation" (s) = the new modeler API, "Optimization" (z) = legacy +
a nested `Solver::Optimization` variant. A reader cannot tell them apart from
a `using` line or an error message.

### The worst spot: `src/solver/optimisation/` (one directory, four namespaces)

```
ComponentToAreaConnectionFiller.h  -> Antares::Optimisation        (new API filler)
ThermalCapacityFiller.h            -> Antares::Optimisation        (new API filler)
opt_fonctions.h                    -> Antares::Optimisation
Legacy*.h  (6 files)               -> Antares::Optimization        (legacy)
csr_quadratic_problem.h            -> Antares::Solver::Optimization
weekly_optimization.h              -> Antares::Solver::Optimization
HebdoProblemToLpsTranslator.h      -> Antares::Solver
MipDetection.h                     -> Antares::Solver
*_post_process_list.h              -> Antares::Solver
```

### Global-namespace leftovers (French / un-scoped)

- `namespace DonneesOptimisationMensuelle` — `src/solver/hydro/.../monthly/`
- `namespace DoneesOptimisationJournaliere` — `src/solver/hydro/.../daily/`
  (note: also a **typo**, "Donees" is missing an `n` — should be "Données")
- ~20 legacy C-style solver headers have **no namespace at all**:
  `sim_structure_probleme_economique.h`, `sim_structure_donnees.h`,
  `h2o_*_donnees_*.h`, `probleme_spx_wrapper.h`, `sim_alloc_probleme_hebdo.h`, …

### Style inconsistency (low priority, cosmetic)

Forward-declaration headers still use nested `namespace Antares { namespace Data {`
rather than the C++17 `namespace Antares::Data`. This is not a scoping bug
(they *are* under `Antares`), just old style. Examples: `study/fwd.h`,
`IStudyLoader.h`, `finalLevelValidator.h`.

### Leave alone (do NOT touch)

- **Generated ANTLR files**: `HoursField*`, `ScenarioBuilder*`, `SymmetryField*`
  under `reservesParsing/`, `additionalConstraintRhsExpression/`,
  `scenarioGroupParser/`. These are regenerated from grammars — editing them
  is pointless.
- **Third-party**: `Yuni::*`, `src/ext/`, `antlr4`, `operations_research`, `YAML`.

---

## Work items

### Item 1 — Remove the two-spellings ambiguity (headline fix)

**Guiding principle (from the user):** the two namespaces are *different
concepts*, not two spellings of one thing. Do **not** merge them into a single
spelling — that would force unrelated code together and re-introduce collisions.
Instead, **give at least one of them a distinct, meaningful name**, and make sure
the two live under **different, non-clashing parents** so `Optimisation` vs
`Optimization` can never again be confused by eye.

Concretely, the two concepts are:

- **(A) The new modeler linear-problem framework** — currently
  `Antares::Optimisation::{LinearProblemApi,LinearProblemDataImpl,LinearProblemMpsolverImpl}`.
- **(B) Legacy solver optimization** — currently `Antares::Optimization` and the
  stray `Antares::Solver::Optimization`.

**Decision (locked): concept (A) becomes `Antares::LinearProblem`.** It loses
the word "Optim*" entirely, so the only remaining "Optim*" token in the tree is
the American `Optimization` (concept B) — the `s`↔`z` ambiguity disappears by
construction.

Concrete target mapping for concept (A):

| Current | New |
|---------|-----|
| `Antares::Optimisation::LinearProblemApi` | `Antares::LinearProblem::Api` |
| `Antares::Optimisation::LinearProblemDataImpl` | `Antares::LinearProblem::DataImpl` |
| `Antares::Optimisation::LinearProblemMpsolverImpl` | `Antares::LinearProblem::MpsolverImpl` |
| bare `Antares::Optimisation` (new fillers in `src/solver/optimisation/`: `ComponentToAreaConnectionFiller`, `ThermalCapacityFiller`, `opt_fonctions`) | `Antares::LinearProblem` |
| `Antares::Optimisation…` in `src/modeler-optimisation-container/` | `Antares::LinearProblem…` |

Short leaves (`::Api`, `::DataImpl`, `::MpsolverImpl`) drop the now-redundant
`LinearProblem` prefix. If a collision surfaces, fall back to keeping the longer
leaf for that symbol only.

After (A) is renamed, **there is no `Optimisation` (British) left anywhere** —
the goal of Item 1 is met. Concept (B) keeps American `Optimization`.
Consolidating the two American variants (`Antares::Optimization` vs the stray
`Antares::Solver::Optimization`) into one parent is nice-to-have and is handled
in Item 2, not required here.

Steps:

1. Confirm no symbol collision between (A) and (B) — grep the leaf class names of
   the new API against the legacy tree. Should be none (they don't share names),
   but verify before mass-renaming.
2. Rename (A) per the table above. Update `#include` paths only if you also rename
   directories (see below) — otherwise leave paths, rename namespace tokens only.
3. Directory renames (`src/optimisation`, `src/modeler-optimisation-container`,
   header paths under `antares/optimisation/…`) are **optional and deferred**:
   they force `CMakeLists.txt` target-name + `target_link_libraries` + every
   `#include` to change. The namespace rename alone removes the ambiguity, so a
   dir rename is a separate, later commit if desired. Note the dir names still say
   "optimisation" — acceptable short-term since the namespace is now unambiguous.

Do this as a **mechanical, per-symbol rename** (not blind sed): the token
`Optimization` also appears in `optimization_statistics.h`, `optimization-options`,
comments and log messages — rename the *namespace* tokens only, verify each hunk.

Rebuild. Fix `using namespace` / qualified-name breakage until green.

### Item 2 — Detangle `src/solver/optimisation/`

Give each file one clearly-correct namespace:

- New modeler fillers (`ComponentToAreaConnectionFiller`, `ThermalCapacityFiller`,
  `opt_fonctions`) → `Antares::LinearProblem` (renamed in Item 1).
- `Legacy*` files → `Antares::Optimization` (American) — leave as legacy.
- `weekly_optimization`, `csr_quadratic_problem` currently `Antares::Solver::Optimization`
  → consolidate the two American variants into a **single** parent (recommend
  `Antares::Solver::Optimization`, moving the bare `Antares::Optimization` legacy
  fillers under it, or vice versa — pick one, don't keep both).
- `HebdoProblemToLpsTranslator`, `MipDetection`, `*_post_process_list` currently
  in bare `Antares::Solver` — decide whether they belong under the optim subtree
  or stay in `Antares::Solver`. Document the choice in a one-line comment.

### Item 3 — Namespace the global-namespace hydro/sim leftovers

Lower priority, higher risk (these are C-style translation units touched by
many `.cpp`).

- Wrap `DonneesOptimisationMensuelle` and `DoneesOptimisationJournaliere` in
  `Antares::Solver::Hydro` (or similar). Fix the `Donees` → `Donnees` typo in
  the same pass, updating all references.
- The un-namespaced `sim_structure_*` / `h2o_*` / `probleme_spx_wrapper`
  headers: wrap in `Antares::Solver::Simulation` **only if** the churn is
  tolerable — every including `.cpp` needs qualification. If risky, defer and
  just document them as known debt.

### Item 4 (optional, cosmetic) — Modernize nesting style

Convert `namespace Antares { namespace Data {` → `namespace Antares::Data` in
the forward-declaration headers. Pure readability; do only if Items 1–3 landed
cleanly and there is appetite for the diff.

---

## Execution guidance for Sonnet

- **One item = one commit**, rebuilt green before the next.
- Rename via editor find-and-replace on **namespace tokens**, never a
  repo-wide `sed s/Optimization/Optimisation/` — that would corrupt
  `optimization_statistics`, option strings, and comments.
- After each item: `cmake --build --target all --preset Debug-vcpkg`, then run
  the relevant unit tests under `src/tests/`.
- Do not touch generated ANTLR files or `Yuni`/`ext`/third-party.
- If a rename reveals a genuine symbol collision, prefer a child namespace over
  renaming the class — smaller blast radius.
