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
│ ConstraintNamer (recording  │              │  └─ AddLegacyExtraOutputs(...)   │
│ ctor) → problem.Legacy-     │              │      ├─ builds LegacySolutionView│
│   ConstraintsInfo[i]        │              │      └─ derived rows             │
└─────────────────────────────┘              └──────────────────────────────────┘
```

1. **Recording.** While the problem is being named, `VariableNamer::RecordLegacyVariableInfo` (a virtual hook on `Namer`, no-op in the base class) fills `PROBLEME_ANTARES_A_RESOUDRE::LegacyVariablesInfo`, a `std::vector<std::optional<LegacyVariableInfo>>` parallel to the solution vector `X` and to the linear objective coefficients `CoutLineaire` (all three are sized to `NombreDeVariables` by `resizeProbleme`). Variables that get no legacy info stay `nullopt` and are ignored by everything downstream.

   Constraints are recorded symmetrically into `LegacyConstraintsInfo` (parallel to the duals vector `CoutsMarginauxDesContraintes`, both sized to `NombreDeContraintes`), but **opt-in**: `ConstraintNamer`'s default constructor records nothing; a constraint type is recorded only where its construction site passes the recording constructor (`ConstraintNamer namer(names, legacyInfo)`), reached through `ConstraintBuilderData::LegacyConstraintsInfo`. Today three constraint types record, because their duals feed outputs: `AreaBalance`, `FlowDissociation` and `FinalStockExpression` (each in its file under `constraints/`).

2. **Raw rows.** After the solve, `FillLegacySimulationTable` iterates `LegacyVariablesInfo` and pushes one `SimulationTableEntry` per recorded variable, translating the legacy variable name to its public output name through `LegacyNameMapper` (e.g. `UnsuppliedEnergy` → `unsupplied_energy`). Recorded constraints produce no raw rows; they exist only for derived outputs.

3. **Derived rows.** The same function then calls `AddLegacyExtraOutputs`, passing the variable info vector, `X`, `CoutLineaire`, the constraint info vector, the duals, the `FillContext` (block time window and scenario year) and the current block index. This is the single entry point for all extra outputs.

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

`AddLegacyExtraOutputs` builds a `LegacySolutionView`, then makes one pass over `LegacyVariablesInfo` and one over `LegacyConstraintsInfo`, dispatching on the recorded name. Each derived output is implemented as a small helper that computes a value and emits a `SimulationTableEntry` via a shared `AddExtraOutputEntry` helper, which fixes the row conventions once: `block = currentBlock + 1`, `absolute_time_index = timeIndex + 1` (1-based, like the raw rows), `block_time_index` relative to the block's global time window, `scenario_index = fillContext.getYear()`.

Outputs implemented so far:

| Output | Driven by | Formula | How operands are read |
|--------|-----------|---------|-----------------------|
| `prop_cost` (thermal cluster) | each `DispatchableProduction` variable | `CoutLineaire[i] × X[i]` | by index |
| `actual_num_units_on` (thermal cluster) | each `NODU` variable | `ceil(X[i])` | by index |
| `imbalance_cost` (area) | each `UnsuppliedEnergy` variable | `unsCost × uns + spillCost × spilled` | unsupplied part by index; `Spillage` of the same component and time through the view |
| `is_loss_of_load` (area) | each `UnsuppliedEnergy` variable | `X[i] > 0.5 ? 1 : 0` | by index |
| `abs_flow` (link) | each `DirectFlow` variable | `abs(X[i])` | by index |
| `prop_cost` (link) | each `PositiveDirectFlow` variable | `directHurdle × posDirect + indirectHurdle × posIndirect` | direct part by index; `PositiveIndirectFlow` of the same link and time through the view |
| `price` (area) | each `AreaBalance` constraint | `-dual[i]` | dual by index |
| `is_near_loss_of_load` (area) | each `AreaBalance` constraint | `-dual[i] > unsCost - 5 ? 1 : 0` | dual by index; the unsupplied energy cost is the objective coefficient on the area's `UnsuppliedEnergy` variable, through the view |
| `capacity_shadow_price` (link) | each `FlowDissociation` constraint | `abs(dual[i])` | dual by index |
| `hydro_shadow_price` (area) | each `FinalStockExpression` constraint | `dual[i]` | dual by index; one row per area and week (constraint exists only in accurate water value mode) |

The `price` formula negates the stored dual: with the legacy balance-constraint sign convention, `CoutsMarginauxDesContraintes` holds the **negative** of the marginal price (the legacy outputs print `-CoutsMarginauxHoraires`, and the adequacy-patch post-processing stores `-unsuppliedEnergyCost` to mean "price = unsupplied energy cost").

Outputs anchored on a recorded constraint read their own dual by index. `FlowDissociation` (hence `capacity_shadow_price`) only exists for links managed with hurdle costs, like the flow decomposition variables.

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

1. Pick the **anchor** — the recorded variable or constraint whose component/time the row should carry — and add a branch for its name in the matching dispatch loop of `AddLegacyExtraOutputs`.
2. If the anchor is a constraint type not yet recorded, switch its construction site (in `src/solver/optimisation/constraints/`) to the recording `ConstraintNamer` constructor, passing `builder.data.LegacyConstraintsInfo`.
3. Write a helper following the existing ones: read the anchor's operands by index; fetch companion variables through `LegacySolutionView` using their **legacy** names; return early (emit nothing) if a companion lookup comes back empty.
4. Emit through `AddExtraOutputEntry` so block/time/scenario conventions stay uniform.
5. If the companion key may not be unique (cluster-level variables), do not use the view — extend the recorded info first.
6. Add a case to `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` and, when the value is deterministically derivable from a test study, a row to the cucumber scenario in `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature`.

## 6. Known Limitations and Planned Extensions

The full extra-output specification (area prices, thermal non-proportional costs and emissions, link congestion fees, storage profits, hydro shadow prices, ...) is being implemented incrementally. The remaining increments need capabilities this design deliberately leaves out for now:

- **Port-based outputs.** Outputs the specification expresses with `in_port`/`out_port` (notably `alg_congestion_fee` and `abs_congestion_fee`, where `price` is read on each end's port) are out of scope for this step. The legacy weekly problem has no port concept, and resolving `in_port`/`out_port` to the right per-area dual is a design question of its own.
- **Profit outputs.** Thermal `profit` needs study data on top of the area price (`cluster_min_gen_modulation`, unit count, unit capacity), and the thermal anchor's component is not area-qualified, so the cluster's area price cannot even be looked up yet. STS `profit` has the same problem: the recorded component is the storage name, not its area. Both are blocked on the component-qualification item below (plus study data for thermal).
- **`bellman_value`.** `sum(cost_layer × LayerStorage)` is computable from `CoutLineaire × X`, but `LayerStorage` variables are recorded with the layer index as component — without the area, the sum cannot be attributed. Also blocked on component qualification.
- **MIP solves.** When the weekly problem is a MIP, OR-Tools' dual extraction is skipped and `CoutsMarginauxDesContraintes` is zero-filled (`extract_from_MPSolver`, a known TODO in `ortools_utils.cpp`), so `price` and `is_near_loss_of_load` read 0 — the same caveat as the legacy marginal-price output.
- **Study data not present in the problem.** Some formulas use parameters that are not objective coefficients (emission rates, cluster availability, link capacities for `is_directly/indirectly_congested`, reservoir capacity for `level_percentage`). These need plumbing from `PROBLEME_HEBDO`/study data into the fill path; `linearCost()` only covers parameters that happen to be objective coefficients.
- **Cross-time terms.** Outputs using `[t-1]` (e.g. thermal `non_prop_cost`) must define their semantics at the first timestep of a block.
- **Component qualification.** As described in §4, cluster components are not area-qualified; an `area` field in `LegacyVariableInfo` would make all keys unique and let cluster-level outputs use the view safely.

## 7. Testing

- `src/tests/src/solver/optimisation/test_legacy_solution_view.cpp` — unit tests of the index: hits, disambiguation by component, misses, unrecorded slots.
- `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` — unit tests of the derived rows: formulas (including the dual sign negation for `price`), row metadata conventions, skip-on-missing-companion, and that unrelated variables and constraints emit nothing. Both run in the `unit-tests-for-solver-optimisation` Boost test target, which compiles the two `.cpp` files directly and links `Antares::simulation-table`.
- `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature` — end-to-end scenario on the "002 Thermal fleet - Base" study at a loss-of-load hour where every cluster is provably at maximum, giving closed-form expected values for `prop_cost`, `imbalance_cost`, `is_loss_of_load`, `price` (= the unsupplied energy cost, since the marginal MW is unserved) and `is_near_loss_of_load`.

Several outputs currently have unit-test coverage only, because no fast test study exercises them: `actual_num_units_on` needs an accurate unit-commitment study (all `hybrid/` studies use `unit-commitment-mode = fast`, where NODU variables do not exist); `abs_flow`, link `prop_cost` and `capacity_shadow_price` need a study with a configured link (with hurdle costs for the last two) — no `hybrid/` study has one; `hydro_shadow_price` needs a study with accurate water value mode. Adding a small two-area study with a hurdle-cost link and a reservoir would close most of the gap.
