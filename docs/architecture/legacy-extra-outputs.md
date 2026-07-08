# Legacy Extra Outputs in the Simulation Table

## 1. Background and Motivation

The legacy (weekly) solver exposes its results in the modeler simulation table in two categories:

- **Raw rows** — one per named optimisation variable, `value = X[i]`. `VariableNamer` records, at problem-naming time, a `LegacyVariableInfo{name, component, timeIndex}` parallel to the variable name (`PROBLEME_ANTARES_A_RESOUDRE::LegacyVariablesInfo`); after each solve `FillLegacySimulationTable` writes one row per recorded variable, translating the legacy name to its public output name through `LegacyNameMapper` (e.g. `UnsuppliedEnergy` → `unsupplied_energy`).
- **Derived rows** — quantities computed from the solution rather than variables of the problem: a thermal cluster's `prop_cost = generation_cost × generation_power`, an area's `price = -dual(balance constraint)`, congestion indicators, emissions, margins, `profit`, ... These are produced by `AddLegacyExtraOutputs` (`src/solver/optimisation/LegacyExtraOutputs.cpp`), the single entry point for all extra outputs.

## 2. Design: iterate the study structure, read by index

`AddLegacyExtraOutputs(simulationTable, problemeHebdo, fillContext, currentBlock)` does not use the recorded variable info at all. The weekly problem's layout is fully known through the correspondence tables, so the function simply iterates the study structure and fetches every operand by index:

```
for pdt in [0, NombreDePasDeTempsPourUneOptimisation):
    for each area:            areaOutputs(pays, pdt)
    for each thermal cluster: thermalOutputs(pays, index, pdt)
    for each link:            linkOutputs(interco, pdt)
for each area:                weeklyHydroOutputs(pays)   # hydro_shadow_price
```

Three sources cover every formula:

| Operand | How it is read |
|---|---|
| Solution value / objective coefficient of a variable | `X[i]` / `CoutLineaire[i]`, with `i` from `VariableManager` (`variables/VariableManagement.h`), e.g. `vm.UnsuppliedEnergy(pays, pdt)`, `vm.DispatchableProduction(palier, pdt)` |
| Dual of a constraint | `CoutsMarginauxDesContraintes[c]`, with `c` from the constraint correspondence tables: `CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays[pays]`, `.NumeroDeContrainteDeDissociationDeFlux[interco]`, `NumeroDeContrainteExpressionStockFinal[pays]` |
| Study data that is not in the problem's objective (capacities, inflows, load, emission factors, unit data) | straight from `PROBLEME_HEBDO`: `ValeursDeNTC[pdt]`, `CaracteristiquesHydrauliques[pays]`, `ConsommationsAbattues[pdt]` + `AllMustRunGeneration[pdt]`, `PaliersThermiquesDuPays[pays]` |

Row conventions are fixed in one place (`LegacyExtraOutputEmitter::emit`): the absolute hour is `weekInTheYear * 168 + pdt` (matching the time index recorded on the raw rows), `block_time_index` comes from `LegacyBlockTimeIndex` (shared with the raw-row loop), `scenario_index = fillContext.getYear()`. Components are the area name, the cluster name, or `origin$$destination` for links.

### Guards

Emitters skip outputs whose anchor does not exist, mirroring the construction sites (`opt_construction_variables_optimisees_lineaire.cpp`, `constraints/`):

- variable indices use the `-1` sentinel where construction writes one (`HydroLevel` for areas without a managed reservoir);
- variables and constraints that exist only in a given mode are guarded by that mode's condition, because their correspondence slots are **not** sentinel-initialised: hurdle-cost links (`CoutDeTransport[interco].IntercoGereeAvecDesCouts` → link `prop_cost`, `capacity_shadow_price`), unit commitment (`OptimisationNotFastMode` → `actual_num_units_on`, `non_prop_cost`), accurate water value (`CaracteristiquesHydrauliques[pays].AccurateWaterValue` → `hydro_shadow_price`);
- per-hour study series are bounds-checked (`ApportNaturelHoraire`, thermal availability / min-generation series).

## 3. Outputs

| Output | Entity | Formula | Operands |
|--------|--------|---------|----------|
| `prop_cost` (thermal) | cluster | `CoutLineaire[ip] × X[ip]` | `ip = vm.DispatchableProduction` |
| `co2_emissions` … `op5_emissions` | cluster | `X[ip] × emission_factor[pollutant]` | `PaliersThermiquesDuPays.emissionFactors` |
| `cluster_availability` | cluster | `max(avail, minStablePower × ceil(avail / unitSize))` | spinning-adjusted quantities of the weekly problem (see §5) |
| `up_margin` | cluster | `cluster_availability − X[ip]` | as above |
| `min_gen_power` | cluster | `min(X[ip], minGen[pdt])` | `PuissanceMinDuPalierThermique` |
| `down_margin` | cluster | `X[ip] − min(cluster_availability, minGen[pdt])` | as above |
| `profit` | cluster | `(price − CoutLineaire[ip]) × max(X[ip] − minGen[pdt], 0)` | area price = balance dual of the cluster's area |
| `actual_num_units_on` | cluster | `ceil(X[in])` | `in = vm.NumberOfDispatchableUnits` (may be fractional when relaxed) |
| `non_prop_cost` | cluster | `startup_cost × started + fixed_cost × ceil(X[in])` | startup cost = coefficient on `NumberStartingDispatchableUnits`; `started` compares `ceil(NODU)` against `pdt−1`, dropped at the interval's first hour |
| `imbalance_cost` | area | `spillCost × spilled + unsCost × unsupplied` | `vm.Spillage`, `vm.UnsuppliedEnergy` |
| `is_loss_of_load` | area | `X[iu] > 0 ? 1 : 0` | strict positivity |
| `is_significant_loss_of_load` | area | `X[iu] > 0.5 ? 1 : 0` | 0.5 MW solver-noise threshold |
| `actual_load` | area | `ConsommationAbattueDuPays + AllMustRunGenerationOfArea` | residual load plus must-run (raw input load) |
| `price` | area | `−dual(AreaBalance)` | see sign note below |
| `is_near_loss_of_load` | area | `price > unsCost − 5 ? 1 : 0` | `unsCost = CoutLineaire[iu]` |
| `level_percentage` | area | `X[ih] / TailleReservoir × 100` | skipped when the capacity is non-positive |
| `actual_inflows` | area | `round(ApportNaturelHoraire[pdt])` | only areas with a managed reservoir |
| `hydro_shadow_price` | area | `dual(FinalStockExpression)` | weekly, accurate water value mode only |
| `abs_flow` | link | `abs(X[idf])` | `idf = vm.DirectFlow` (signed) |
| `minus_flow` | link | `−X[idf]` | GEMS sign convention |
| `actual_loop_flow` | link | `ValeurDeLoopFlowOrigineVersExtremite[interco]` | input parameter |
| `is_directly_congested` | link | `X[idf] ≥ directNTC − 1e-5 ? 1 : 0` | per-pdt NTC |
| `is_indirectly_congested` | link | `−X[idf] ≥ indirectNTC − 1e-5 ? 1 : 0` | per-pdt NTC |
| `abs_congestion_fee` | link | `abs(flow) × abs(price_out − price_in)` | endpoint balance duals |
| `alg_congestion_fee` | link | `flow × (price_out − price_in)` | as above |
| `prop_cost` (link) | link | `directHurdle × posDirect + indirectHurdle × posIndirect` | hurdle costs are the coefficients on the flow decomposition variables; hurdle-cost links only |
| `capacity_shadow_price` | link | `abs(dual(FlowDissociation))` | hurdle-cost links only |

The `price` formula negates the stored dual: with the legacy balance-constraint sign convention, `CoutsMarginauxDesContraintes` holds the **negative** of the marginal price (the legacy outputs print `-CoutsMarginauxHoraires`).

## 4. How to Add a New Extra Output

1. Pick the entity loop the output belongs to (`areaOutputs`, `linkOutputs`, `thermalOutputs`, or a weekly method) in `LegacyExtraOutputs.cpp`.
2. Read the operands by index: variables through `VariableManager`, duals through the constraint correspondence tables, study data straight from `PROBLEME_HEBDO`.
3. If the anchor only exists in a given mode, guard with the same condition as its construction site.
4. Emit through `emit()` so the row conventions stay uniform.
5. Add a case to `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` (the fixture is a minimal hand-built `PROBLEME_HEBDO`) and, when the value is deterministically derivable from a test study, a row to the cucumber scenario in `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature`.

## 5. Known Caveats

- **Cost noise.** `CoutLineaire` holds the costs the optimizer actually used, which the legacy solver deliberately perturbs to break degeneracy (`PrepareRandomNumbers` in `src/solver/simulation/common-eco-adq.cpp`); each thermal cost gets a noise whose absolute value is forced into `[5e-4, 6e-4]` even with a zero `spread-cost`, and the unsupplied/spilled costs get the same treatment. Every cost-derived output (`prop_cost`, `imbalance_cost`, `non_prop_cost`, `profit`) deviates from the theoretical `study_cost × quantity` accordingly (relative error ≈ `6e-4 / cheapest_cost`); tests comparing against theoretical values must allow for this (the cucumber scenario uses a relative tolerance of `1e-4`). The quantities themselves (`X`) are unaffected.
- **MIP solves.** When the weekly problem is a MIP, OR-Tools' dual extraction is skipped and `CoutsMarginauxDesContraintes` is zero-filled (`extract_from_MPSolver`, a known TODO in `ortools_utils.cpp`), so every dual-derived output (`price`, `is_near_loss_of_load`, fees, shadow prices, `profit`) reads 0 — the same caveat as the legacy marginal-price output.
- **Spinning and the margin formulas.** The thermal availability series and unit size are already multiplied by `(1 − spinning/100)` at load time (`cluster.cpp::calculationOfSpinning`). The spec writes `cluster_availability` with the raw quantities and re-applies the factor; algebraically it cancels, so the formulas use the spinning-adjusted quantities the problem already holds.
- **Cross-time terms.** `non_prop_cost` uses `NODU[t−1]`; at the first hour of an optimisation interval the previous value is not part of the solution and the start-up term is dropped.
- **Port-based outputs.** Outputs the specification expresses with `in_port`/`out_port` semantics beyond the endpoint-price congestion fees are out of scope; the legacy weekly problem has no port concept.

## 6. Testing

- `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` — unit tests of the derived rows on a minimal hand-built `PROBLEME_HEBDO` (three areas, two links, one cluster): formulas (including the dual sign negation for `price`), row metadata conventions, and the mode guards (fast mode, hurdle costs, reservoir management, accurate water value). Runs in the `unit-tests-for-solver-optimisation` Boost target, which compiles `LegacyExtraOutputs.cpp` and the `VariableManager` sources directly.
- `src/tests/src/solver/optimisation/constraints/legacy_variable_info_namer.cpp` — unit tests of the raw-row recording (`VariableNamer` → `LegacyVariableInfo`).
- `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature` — end-to-end scenarios: "002 Thermal fleet - Base" at a loss-of-load hour (closed-form `prop_cost`, `imbalance_cost`, `is_loss_of_load`, `price`, `is_near_loss_of_load`), "008 Thermal fleet - Accurate unit commitment" (`actual_num_units_on`, emissions, margins), the `Hurdle-cost link` fixture (`abs_flow`, link `prop_cost`, `capacity_shadow_price`, congestion indicators) and the `Accurate hydro pricing` fixture (`hydro_shadow_price`, `level_percentage`).
