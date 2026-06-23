# Legacy Extra Outputs in the Simulation Table

## 1. Background and Motivation

The legacy (weekly) solver exposes its results in the modeler simulation table by recording, at problem-naming time, which optimisation variable corresponds to which physical quantity. `FillLegacySimulationTable` then writes one row per recorded variable after each weekly solve: the *raw* outputs (`unsupplied_energy`, `generation_power`, ...).

A second category of outputs is *derived*: they are not variables of the linear problem but quantities computed from the solution, such as a thermal cluster's proportional cost (`prop_cost = generation_cost × generation_power`) or an area's `imbalance_cost` (`unsupplied_energy_cost × unsupplied_energy + spilled_energy_cost × spilled_energy`). Some of these combine several variables of the same week, which raises a need the raw path does not have: finding a variable of the solved problem **by name** rather than by index.

Two components implement this:

| Component | Files | Role |
|-----------|-------|------|
| `LegacySolutionView` | `src/solver/optimisation/include/antares/solver/optimisation/LegacySolutionView.h`, `src/solver/optimisation/LegacySolutionView.cpp` | Read-only lookup of solution values and linear objective coefficients by (variable name, component, time index) |
| `LegacyExtraOutputs` | `src/solver/optimisation/include/antares/solver/optimisation/LegacyExtraOutputs.h`, `src/solver/optimisation/LegacyExtraOutputs.cpp` | Computes the derived outputs and appends them to the `SimulationTable` |

Both live in namespace `Antares::Optimization` and are part of the `model_antares` target.

## 2. Where They Sit in the Data Flow

```
opt_rename_problem.cpp                       opt_appel_solveur_lineaire.cpp
┌─────────────────────────────┐              ┌──────────────────────────────────┐
│ VariableNamer::             │              │ FillLegacySimulationTable        │
│ RecordLegacyVariableInfo    │   weekly     │  ├─ raw rows: one per recorded   │
│  → problem.LegacyVariables- │ ──solve────► │  │  variable, value = X[i],      │
│    Info[i] = {name,         │              │  │  output mapped by             │
│    component, timeIndex}    │              │  │  LegacyNameMapper             │
└─────────────────────────────┘              │  └─ AddLegacyExtraOutputs(...)   │
                                             │      ├─ builds LegacySolutionView│
                                             │      └─ derived rows             │
                                             └──────────────────────────────────┘
```

1. **Recording.** While the problem is being named, `VariableNamer::RecordLegacyVariableInfo` (a virtual hook on `Namer`, no-op in the base class) fills `PROBLEME_ANTARES_A_RESOUDRE::LegacyVariablesInfo`, a `std::vector<std::optional<LegacyVariableInfo>>` parallel to the solution vector `X` and to the linear objective coefficients `CoutLineaire` (all three are sized to `NombreDeVariables` by `resizeProbleme`). Variables that get no legacy info stay `nullopt` and are ignored by everything downstream.

2. **Raw rows.** After the solve, `FillLegacySimulationTable` iterates `LegacyVariablesInfo` and pushes one `SimulationTableEntry` per recorded variable, translating the legacy variable name to its public output name through `LegacyNameMapper` (e.g. `UnsuppliedEnergy` → `unsupplied_energy`).

3. **Derived rows.** The same function then calls `AddLegacyExtraOutputs`, passing the info vector, `X`, `CoutLineaire`, the `FillContext` (block time window and scenario year) and the current block index. This is the single entry point for all extra outputs.

## 3. LegacySolutionView

`LegacySolutionView` is a transient, read-only index over one solved weekly problem. Its constructor walks `LegacyVariablesInfo` once and builds an `std::unordered_map` from the key `(name, component, timeIndex)` to the variable index; `nullopt` slots are skipped. It offers two accessors, both returning `std::optional<double>` (empty when the key is unknown):

- `value(name, component, timeIndex)` — the solution value `X[i]`;
- `linearCost(name, component, timeIndex)` — the objective coefficient `CoutLineaire[i]`. For most legacy variables this *is* the corresponding cost parameter (e.g. the coefficient on `UnsuppliedEnergy` is the area's unsupplied-energy cost), which lets derived outputs read cost parameters without plumbing study data into the fill path.

Design points:

- **Lookups use legacy names** (`"Spillage"`, not `"spilled_energy"`). The view indexes what was recorded; `LegacyNameMapper` is an output-formatting concern applied only when rows are written.
- **Key encoding.** The map key is the three parts joined with the ASCII unit separator `'\x1F'`, a character that cannot occur in variable or component names, so distinct triples can never produce the same key (component names may contain spaces and the `::` used by problem naming).
- **Transient by design.** The view holds references to the problem's vectors and is rebuilt on every fill. Every weekly problem has the same variables and constraints at the same indices — `PROBLEME_ANTARES_A_RESOUDRE` is re-filled and re-solved in place — but the recorded `timeIndex` is the **absolute hour of the year** (`weekInTheYear * 168 + pdt`, see `opt_construction_variables_optimisees_lineaire.cpp`), so the keys differ from week to week and the map cannot be reused as-is.
- **Cost.** One O(#variables) pass to build, O(1) average per lookup, one heap-allocated map per week. The map only contains recorded variables, a small subset of the problem. Because the variable layout is identical across weeks, keying on the week-relative hour instead of the absolute one would allow building the map once per simulation; this trade-off has not been needed so far.

## 4. LegacyExtraOutputs

`AddLegacyExtraOutputs` builds a `LegacySolutionView`, then makes a single pass over `LegacyVariablesInfo` and dispatches on the recorded variable name. Each derived output is implemented as a small helper that computes a value and emits a `SimulationTableEntry` via a shared `AddExtraOutputEntry` helper, which fixes the row conventions once: `block = currentBlock`, `absolute_time_index = timeIndex` (0-based, like the raw rows), `block_time_index` relative to the block's global time window, `scenario_index = fillContext.getYear()`.

Outputs implemented so far:

| Output | Driven by | Formula | How operands are read |
|--------|-----------|---------|-----------------------|
| `prop_cost` (thermal cluster) | each `DispatchableProduction` variable | `CoutLineaire[i] × X[i]` | by index |
| `actual_num_units_on` (thermal cluster) | each `NODU` variable | `ceil(X[i])` | by index |
| `imbalance_cost` (area) | each `UnsuppliedEnergy` variable | `unsCost × uns + spillCost × spilled` | unsupplied part by index; `Spillage` of the same component and time through the view |
| `is_loss_of_load` (area) | each `UnsuppliedEnergy` variable | `X[i] > 0.5 ? 1 : 0` | by index |
| `abs_flow` (link) | each `DirectFlow` variable | `abs(X[i])` | by index |
| `prop_cost` (link) | each `PositiveDirectFlow` variable | `directHurdle × posDirect + indirectHurdle × posIndirect` | direct part by index; `PositiveIndirectFlow` of the same link and time through the view |

The hurdle costs in the link `prop_cost` formula are the objective coefficients on the flow decomposition variables (`opt_gestion_des_couts_cas_lineaire.cpp` sets them straight from the link's `fhlHurdlesCostDirect`/`fhlHurdlesCostIndirect` series); those variables only carry legacy info for links managed with hurdle costs, so the output is naturally restricted to them. Link components are recorded as `origin$$destination`, a unique key safe for view lookups.

The "driven by" column matters: each derived row is anchored on one recorded variable, which supplies the component and time index of the emitted row. When a required companion variable is missing from the view (e.g. no `Spillage` recorded for the area), the output is skipped for that anchor rather than emitting a partial value.

### Cost noise

`CoutLineaire` holds the costs the optimizer actually used, and those are **not** the raw study costs: the legacy solver deliberately perturbs them to break degeneracy (`PrepareRandomNumbers` in `src/solver/simulation/common-eco-adq.cpp`). Each thermal cluster cost gets a noise whose absolute value is forced into `[5e-4, 6e-4]` even when the cluster's `spread-cost` is 0 (with a non-zero spread, the noise is `(rnd − 0.5) × spread`), and the unsupplied/spilled energy costs get the same treatment driven by the area's `spread-unsupplied-energy-cost`/`spread-spilled-energy-cost`. Consequently every cost-derived output (`prop_cost`, `imbalance_cost`) deviates from the theoretical `study_cost × quantity` by a relative error of up to roughly `6e-4 / cheapest_cost` — about `2e-5` for typical thermal costs. The quantities themselves (`X`) are unaffected. Tests comparing these outputs against theoretical values must allow for this (the cucumber scenario uses a relative tolerance of `1e-4`); producing noise-free costs would require reading study data instead of objective coefficients (§6).

### By-index versus by-name reads

When the value needed belongs to the anchor variable itself, helpers read `X[index]` / `CoutLineaire[index]` directly instead of going through the view. Besides being cheaper, this sidesteps a known limitation of the recorded info: **thermal cluster components are recorded as the cluster name only, not qualified by area** (`sim_calcul_economique.cpp` stores `cluster->name()`). Two identically-named clusters in different areas therefore collide in the by-name index — last writer wins. The rule is:

- per-variable outputs (like `prop_cost`) never use the view;
- the view is only used for keys known to be unique, currently area-level variables (one `Spillage`/`UnsuppliedEnergy` per area per timestep).

Lifting the limitation would mean adding an area qualifier to `LegacyVariableInfo` at recording time; see §6.

## 5. How to Add a New Extra Output

1. Pick the **anchor variable** — the recorded legacy variable whose component/time the row should carry — and add a branch for its name in the dispatch loop of `AddLegacyExtraOutputs`.
2. Write a helper following the existing two: read the anchor's operands by index; fetch companion variables through `LegacySolutionView` using their **legacy** names; return early (emit nothing) if a companion lookup comes back empty.
3. Emit through `AddExtraOutputEntry` so block/time/scenario conventions stay uniform.
4. If the companion key may not be unique (cluster-level variables), do not use the view — extend the recorded info first.
5. Add a case to `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` and, when the value is deterministically derivable from a test study, a row to the cucumber scenario in `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature`.

## 6. Known Limitations and Planned Extensions

The full extra-output specification (area prices, thermal non-proportional costs and emissions, link congestion fees, storage profits, hydro shadow prices, ...) is being implemented incrementally. The remaining increments need capabilities this design deliberately leaves out for now:

- **Constraint duals.** Outputs such as area `price` (dual of the balance constraint) or `capacity_shadow_price` require recording constraint info symmetrically to variables: a `LegacyConstraintsInfo` vector filled by a `ConstraintNamer` hook, read against `CoutsMarginauxDesContraintes`. `ConstraintNamer` records nothing today.
- **Study data not present in the problem.** Some formulas use parameters that are not objective coefficients (emission rates, cluster availability, hurdle costs). These need plumbing from `PROBLEME_HEBDO`/study data into the fill path; `linearCost()` only covers parameters that happen to be objective coefficients — and those carry the anti-degeneracy cost noise described in §4, so the same plumbing would be needed if noise-free costs were ever required.
- **Cross-time terms.** Outputs using `[t-1]` (e.g. thermal `non_prop_cost`) must define their semantics at the first timestep of a block.
- **Component qualification.** As described in §4, cluster components are not area-qualified; an `area` field in `LegacyVariableInfo` would make all keys unique and let cluster-level outputs use the view safely.

## 7. Testing

- `src/tests/src/solver/optimisation/test_legacy_solution_view.cpp` — unit tests of the index: hits, disambiguation by component, misses, unrecorded slots.
- `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` — unit tests of the derived rows: formulas, row metadata conventions, skip-on-missing-companion, and that unrelated variables emit nothing. Both run in the `unit-tests-for-solver-optimisation` Boost test target, which compiles the two `.cpp` files directly and links `Antares::simulation-table`.
- `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature` — end-to-end scenario on the "002 Thermal fleet - Base" study at a loss-of-load hour where every cluster is provably at maximum, giving closed-form expected values for `prop_cost`, `imbalance_cost` and `is_loss_of_load`.

Three outputs currently have unit-test coverage only, because no fast test study exercises them: `actual_num_units_on` needs an accurate unit-commitment study (all `hybrid/` studies use `unit-commitment-mode = fast`, where NODU variables do not exist), and `abs_flow` / link `prop_cost` need a study with a configured link (with hurdle costs for the latter); no `hybrid/` study has one. Adding a small two-area study with a hurdle-cost link would close the gap.
