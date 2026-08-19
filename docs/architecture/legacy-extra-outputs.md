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
                              shortTermStorageOutputs(pays, pdt)
                              inputGenerationOutputs(pays, pdt)  # renewable / misc gen
    for each thermal cluster: thermalOutputs(pays, index, pdt)
    for each link:            linkOutputs(interco, pdt)
for each area:                weeklyHydroOutputs(pays)   # hydro_shadow_price, bellman_value
```

Three sources cover every formula:

| Operand | How it is read |
|---|---|
| Solution value / objective coefficient of a variable | `X[i]` / `CoutLineaire[i]`, with `i` from `VariableManager` (`variables/VariableManagement.h`), e.g. `vm.UnsuppliedEnergy(pays, pdt)`, `vm.DispatchableProduction(palier, pdt)` |
| Dual of a constraint | `CoutsMarginauxDesContraintes[c]`, with `c` from the constraint correspondence tables: `CorrespondanceCntNativesCntOptim[pdt].NumeroDeContrainteDesBilansPays[pays]`, `.NumeroDeContrainteDeDissociationDeFlux[interco]`, `NumeroDeContrainteExpressionStockFinal[pays]` |
| Study data that is not in the problem's objective (capacities, inflows, load, emission factors, unit data) | straight from `PROBLEME_HEBDO`: `ValeursDeNTC[pdt]`, `CaracteristiquesHydrauliques[pays]`, `ConsommationsAbattues[pdt]` + `AllMustRunGeneration[pdt]`, `PaliersThermiquesDuPays[pays]` |

Row conventions are fixed in one place (`LegacyExtraOutputEmitter::emit`): the absolute hour is `weekInTheYear * 168 + pdt` (matching the time index recorded on the raw rows), `block_time_index` comes from `LegacyBlockTimeIndex` (shared with the raw-row loop), `scenario_index = fillContext.getYear()`. Components follow the uniqueness convention: `{area}_node` for area rows, `{area}_hydro_storage` for the hydro storage, `{area}_thermal_{cluster}` for thermal clusters, `{area}_short_term_storage_{name}` for short-term storages, `{origin}_{destination}_link` for links (endpoints sorted), `{area}_load` / `{area}_hydro_storage` for the load / long-term-storage port rows.

### Guards

Emitters skip outputs whose anchor does not exist, mirroring the construction sites (`opt_construction_variables_optimisees_lineaire.cpp`, `constraints/`):

- variable indices use the `-1` sentinel where construction writes one (`HydroLevel` for areas without a managed reservoir);
- variables and constraints that exist only in a given mode are guarded by that mode's condition, because their correspondence slots are **not** sentinel-initialised: hurdle-cost links (`CoutDeTransport[interco].IntercoGereeAvecDesCouts` → link `prop_cost`, `capacity_shadow_price`), unit commitment (`OptimisationNotFastMode` → `actual_num_units_on`, `non_prop_cost`), accurate water value (`CaracteristiquesHydrauliques[pays].AccurateWaterValue` → `hydro_shadow_price`);
- per-hour study series are bounds-checked (`ApportNaturelHoraire`, thermal availability / min-generation series);
- structurally inactive objects (an entirely-zero series, a link with no capacity) are guarded by the precomputed `InactiveComponentsAnalyzer` — see §7.

## 3. Outputs

| Output | Entity | Formula | Operands |
|--------|--------|---------|----------|
| `prop_cost` (thermal) | cluster | `marginalCost × X[ip]` | `ip = vm.DispatchableProduction`; `marginalCost = CoutMarginalDeProductionDuPalierThermique[pdt]` |
| `co2_emissions` … `op5_emissions` | cluster | `X[ip] × emission_factor[pollutant]` | `PaliersThermiquesDuPays.emissionFactors` |
| `cluster_availability` | cluster | `max(avail, minStablePower × ceil(avail / unitSize))` | spinning-adjusted quantities of the weekly problem (see §5) |
| `up_margin` | cluster | `cluster_availability − X[ip]` | as above |
| `min_gen_power` | cluster | `min(X[ip], minGen[pdt])` | `PuissanceMinDuPalierThermique` |
| `down_margin` | cluster | `X[ip] − min(cluster_availability, minGen[pdt])` | as above |
| `profit` | cluster | `(price − CoutLineaire[ip]) × max(X[ip] − minGen[pdt], 0)` | area price = balance dual of the cluster's area |
| `actual_num_units_on` | cluster | `ceil(X[in])` | `in = vm.NumberOfDispatchableUnits` (may be fractional when relaxed) |
| `non_prop_cost` | cluster | `startup_cost × started + fixed_cost × ceil(X[in])` | startup cost = coefficient on `NumberStartingDispatchableUnits`; `started` compares `ceil(NODU)` against `pdt−1`, dropped at the interval's first hour |
| `imbalance_cost` | area | `spillCost × spilled + unsCost × unsupplied` | `vm.Spillage`, `vm.UnsuppliedEnergy`; costs = `CoutDeDefaillanceNegative/PositiveSansBruit[pays]`, the user costs without noise |
| `is_loss_of_load` | area | `X[iu] > 0 ? 1 : 0` | strict positivity |
| `is_significant_loss_of_load` | area | `X[iu] > 0.5 ? 1 : 0` | 0.5 MW solver-noise threshold |
| `actual_load` | `{area}_load` | `ConsommationAbattueDuPays + AllMustRunGenerationOfArea` | residual load plus must-run (raw input load) |
| `price` | area | `−dual(AreaBalance)` | see sign note below |
| `is_near_loss_of_load` | area | `price > unsCost − 5 ? 1 : 0` | `unsCost = CoutLineaire[iu]` |
| `level_percentage` | area | `X[ih] / TailleReservoir × 100` | skipped when the capacity is non-positive |
| `actual_inflows` | area | `round(ApportNaturelHoraire[pdt])` | only areas with a managed reservoir |
| `hydro_shadow_price` | area | `-dual(FinalStockExpression)` | weekly, accurate water value mode only |
| `bellman_value` | area | `−Σ_layer CoutLineaire[il] × X[il]` | `il = vm.LayerStorage(pays, layer)`; weekly, accurate water value mode only |
| `profit` | storage | `floor((X[iw] − X[ii]) × price + 0.5)` | `iw/ii = vm.ShortTermStorageWithdrawal/Injection(clusterGlobalIndex)`; price = balance dual of the storage's area |
| `generation_power` | renewable / misc gen | `availablePower[pdt]` | `InputGenerationOfArea[pays]`, filled from study series by `SIM_RenseignementProblemeHebdo` (not an LP variable) |
| `minus_generation` | renewable / misc gen | `−availablePower[pdt]` | as above |

The §2.5 port fields are emitted by the same functions, with the port field name
as the ST output: `balance_port.price` (area, = `price`), `out_port.flow` /
`in_port.flow` (link, = `±flow`), and `balance_port.flow` for thermal
(`generation_power`), short-term storage (`withdrawal − injection`, component
`{area}_short_term_storage_{name}`), renewable / misc gen (`availablePower`),
load (`−rawLoad`, component `{area}_load`) and long-term storage
(`X[HydProd] − X[Pumping]`, component `{area}_hydro_storage`, skipped without
hydro production). The `_load` / `_hydro_storage` suffixes keep the two
area-level `balance_port.flow` rows from colliding on the area name.
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

- **Cost noise.** `CoutLineaire` holds the costs the optimizer actually used, which the legacy solver deliberately perturbs to break degeneracy (`PrepareRandomNumbers` in `src/solver/simulation/common-eco-adq.cpp`); each thermal cost gets a noise whose absolute value is forced into `[5e-4, 6e-4]` even with a zero `spread-cost`, and the unsupplied/spilled costs get the same treatment. Thermal `prop_cost` and `imbalance_cost` therefore do **not** read `CoutLineaire`: they use the un-noised user costs kept alongside the noised ones (`CoutHoraireDeProductionDuPalierThermiqueSansBruit`, `CoutDeDefaillancePositive/NegativeSansBruit`), so they match the theoretical `study_cost × quantity` exactly. Outputs still derived from the perturbed optimisation (`non_prop_cost` via `CoutLineaire`, `is_near_loss_of_load`'s VoLL threshold, and every dual-derived value such as `price` and `profit`) deviate accordingly; the noise can also shift the dispatch itself between equal-cost solutions. The quantities (`X`) are otherwise unaffected.
- **MIP solves.** When the weekly problem is a MIP, OR-Tools' dual extraction is skipped and `CoutsMarginauxDesContraintes` is zero-filled (`extract_from_MPSolver`, a known TODO in `ortools_utils.cpp`), so every dual-derived output (`price`, `is_near_loss_of_load`, fees, shadow prices, `profit`) reads 0 — the same caveat as the legacy marginal-price output.
- **Spinning and the margin formulas.** The thermal availability series and unit size are already multiplied by `(1 − spinning/100)` at load time (`cluster.cpp::calculationOfSpinning`). The spec writes `cluster_availability` with the raw quantities and re-applies the factor; algebraically it cancels, so the formulas use the spinning-adjusted quantities the problem already holds.
- **Cross-time terms.** `non_prop_cost` uses `NODU[t−1]`; at the first hour of an optimisation interval the previous value is not part of the solution and the start-up term is dropped.
- **Port-based outputs.** Outputs the specification expresses with `in_port`/`out_port` semantics beyond the endpoint-price congestion fees are out of scope; the legacy weekly problem has no port concept.

## 6. Testing

- `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` — unit tests of the derived rows on a minimal hand-built `PROBLEME_HEBDO` (three areas, two links, one cluster): formulas (including the dual sign negation for `price`), row metadata conventions, and the mode guards (fast mode, hurdle costs, reservoir management, accurate water value). Runs in the `unit-tests-for-solver-optimisation` Boost target, which compiles `LegacyExtraOutputs.cpp` and the `VariableManager` sources directly.
- `src/tests/src/solver/optimisation/test_legacy_extra_outputs.cpp` (post-process cases) — unit tests of the per-stage dump (§8) on the same fixture: that the dump reads results mutated *after* the solve, that it leaves `X` and the duals exactly as it found them, that it no-ops when the optimisation range is daily, and that the block index is derived from the hour in the year. The `X` and dual refreshes are checked to be independently exercised — disabling only the `X` refresh must break assertions that the dual refresh alone still satisfies.
- `src/tests/src/io/outputs/testSimulationTable.cpp` — unit tests of the stage map: a stage is created on first use and its pointer stays valid once later stages are added, the writer names one file per stage, and a stage with no rows is skipped rather than written as a header-only file. Also the stage selection (§8): every stage name parses, `all` and an empty list mean no restriction, spaces around commas are tolerated, an unknown name (including a prefix of a real one) throws, an unselected stage returns `nullptr` and is never created, and the writer emits no file for it.
- `src/tests/src/solver/optimisation/constraints/legacy_variable_info_namer.cpp` — unit tests of the raw-row recording (`VariableNamer` → `LegacyVariableInfo`).
- `src/tests/src/solver/optimisation/test_inactive_components_analyzer_builder.cpp` — unit tests of `BuildInactiveComponentsAnalyzer` against an in-memory `Data::Study` (via the `StudyBuilder`/`TimeSeriesConfigurer` test helpers), confirming a component is judged inactive when only its selected chronicle is all-zero, even if the series matrix has other, non-zero, unselected columns.
- `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature` — end-to-end scenarios: "002 Thermal fleet - Base" at a loss-of-load hour (closed-form `prop_cost`, `imbalance_cost`, `is_loss_of_load`, `price`, `is_near_loss_of_load`), "008 Thermal fleet - Accurate unit commitment" (`actual_num_units_on`, emissions, margins), the `Hurdle-cost link` fixture (`abs_flow`, link `prop_cost`, `capacity_shadow_price`, congestion indicators) and the `Accurate hydro pricing` fixture (`hydro_shadow_price`, `level_percentage`). The same feature also covers §7's suppression rules (zero-series load/ROR/solar/wind/misc-gen, unmanaged+dry hydro, zero-NTC links). It also covers §8's per-stage tables on the `adq-patch-CSR-test-case-v02` fixture: the set of stage files produced (which pins the stage names), and — reading from the `adq-patch-csr` stage — the curtailment-sharing redistribution of unsupplied energy between the two *inside* areas, the matching shift of the link flow, and the post-CSR `price`, i.e. one assertion on each of the two halves of the refresh. Four further scenarios cover the stage selection: a two-stage command-line selection, which asserts both the restricted file set *and* that the surviving stages still carry the values the unrestricted run produces; a single-stage selection; a study that asks for its stages in `generaldata.ini`; and `--simulation-table-stages=all` overriding a study that restricts them.

## 7. Suppressing Rows for Structurally Inactive Components

To keep the simulation table light, rows are omitted for objects that don't meaningfully participate in the simulation: an area's load/ROR/solar/wind/misc-gen component whose series is entirely zero, a hydro reservoir that is both unmanaged and dry, and a link with no exchange capacity in either direction. This is an **output-only** change — it does not alter what gets computed or optimised, only what gets written to the simulation table.

### Design: a study-wide, once-only precomputed analyzer

"Inactive" status for anything driven by time-series content is decided **once per study**, before the per-MC-year simulation loop — never re-evaluated per week or per MC year. Re-deciding per week/year would make the simulation table's column set drift between weeks or years for the same object, which breaks downstream tools that expect a stable schema.

- `InactiveComponentsAnalyzer` (`src/solver/optimisation/include/antares/solver/optimisation/InactiveComponentsAnalyzer.h`) is a plain, `Data::Study`-independent lookup: flat `bool` flags per area (`load`/`ror`/`solar`/`wind`/`hydroInflow`), per area+misc-gen-column (8 columns), and per link, indexed exactly like `PROBLEME_HEBDO`'s `pays`/`interco`. Default-constructed, every flag reads `false` (active), so a `PROBLEME_HEBDO` fixture that never sets one is unaffected — this is what keeps every guard below optional/null-safe.
- `BuildInactiveComponentsAnalyzer(const Data::Study&)` (`InactiveComponentsAnalyzerBuilder.h/.cpp`) computes the flags. For load/ROR/solar/wind/hydro-inflow and link capacities, a chronicle-bearing series can have several columns, but only some of them are ever drawn for this study's Monte-Carlo years — the draw is made once, study-wide, before the simulation loop (`TimeSeriesNumbers::Generate`, `src/solver/simulation/timeseries-numbers.cpp`), so it is itself stable across weeks/years and safe to consult here. The private helper `selectedColumnsAreAllZero(const Data::TimeSeries&)` walks `ts.timeseriesNumbers[year]` for every MC year and checks only those selected columns are all-zero — a non-zero chronicle that exists in the file but is never selected does not keep the component active. This applies to `load.series`/`hydro.series->ror`/`solar.series`/`wind.series`/`hydro.series->storage` (the inflow, aka "apports", series — distinct from `ror`) and to `directCapacities`/`indirectCapacities` on each link. Misc-gen is the one exception: its 8 columns are fixed categories with no TS-number/scenario concept (every column is summed every year, see `Area::Scratchpad`), so it keeps the simpler whole-matrix `Matrix<T,ReadWriteT>::containsOnlyZero()` (`matrix.hxx`) check, one call per column (`area.miscGen[column]`, ordered like `Data::MiscGenIndex`). A link is flagged inactive when **both** `directCapacities` and `indirectCapacities` have all-zero selected chronicles (there is no explicit "disabled" flag on `AreaLink`, so this is the closest structural equivalent).
- Areas and links are visited by walking `study.areas` then, per area, `area.links`, in the exact nested order `SIM_InitialisationProblemeHebdo`/`StudyRuntimeInfosInitializeAreaLinks` use to number `pays`/`interco` (both alphabetical), so the resulting flat flag vectors line up 1:1 with `PROBLEME_HEBDO`'s indices without any origin metadata on the `Component`/legacy objects.
- `PROBLEME_HEBDO::inactiveComponents` (`std::shared_ptr<const InactiveComponentsAnalyzer>`, default null) carries the analyzer into `AddLegacyExtraOutputs`. It is built once and attached to every `numSpace`'s `PROBLEME_HEBDO` in `Economy::simulationBegin()` / `Adequacy::simulationBegin()`, before the per-week solve loop — never touching `SIM_InitialisationProblemeHebdo`'s own construction of the optimisation inputs.

### The three guards

All three follow the file's existing "skip when the anchor/condition doesn't hold" style (§ Guards):

- **Load / ROR / solar / wind / misc-gen** (`areaOutputs`'s `actual_load` + `{area}_load` `balance_port.flow`; `inputGenerationOutputs`'s `generation_power`/`minus_generation`/`balance_port.flow` per `InputGenerationOfArea` entry): each `InputGenerationOfArea` entry's `componentName` suffix (`_wind`, `_solar`, `_run_of_river`, or one of the 8 misc-gen suffixes in `LegacyExtraOutputs.cpp`'s `miscGenSuffixes`) picks the matching analyzer flag (`windIsAllZero`/`solarIsAllZero`/`rorIsAllZero`/`miscGenColumnIsAllZero`); load uses `loadIsAllZero` directly. Each misc-gen sub-component is suppressed independently of its 7 siblings.
- **Hydro storage `balance_port.flow`** (`areaOutputs`, the `{area}_hydro` port row anchored on `HydProd`): suppressed only when **both** `!CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire` (reservoir unmanaged) **and** `inactiveComponents->hydroInflowIsAllZero(pays)` hold. `HydProd`'s existence is guarded by `PresenceDHydrauliqueModulable`, a condition independent of `SuiviNiveauHoraire` — an unmanaged reservoir can still have legitimate turbine generation from a fixed/inflow-driven profile, so `reservoirManagement == false` alone must not suppress this row (unlike `level_percentage`/`actual_inflows`, which are already guarded by `HydroLevel`'s existence, i.e. `reservoirManagement` alone).
- **Links** (`linkOutputs`, entire function): the whole row set for a link is skipped, mirroring the `hydroLevel < 0` early-return style, when `inactiveComponents->linkIsAllZero(interco)`.

### Scope: derived rows only, not raw rows

These guards live entirely inside `AddLegacyExtraOutputs` and therefore only suppress **derived** rows (§1). The **raw** per-variable rows (`FillLegacySimulationTable`/`VariableNamer`, e.g. a link's `flow`/`direct_flow`/`indirect_flow`) are a separate mechanism, unaffected: an LP variable still exists (and still gets a raw row) as long as its construction-site condition holds, regardless of whether its value is structurally always zero. A zero-NTC link's `DirectFlow`/`IndirectFlow` variables are still built (the optimizer still needs to prove the flow is zero), so their raw rows remain; only the derived rows (`abs_flow`, `prop_cost`, port fields, congestion indicators, ...) are dropped.

## 8. Per-Stage Simulation Tables

A week is not resolved in one shot: two optimisation passes are followed by post-treatments that move the results without re-solving (shave-peaks / remix hydro, and — when the adequacy patch is on — curtailment sharing, DTG netting and the marginal price update). The simulation table is written **once per stage**, so the effect of each treatment is observable.

| Stage | File | Written after | Produced in |
|---|---|---|---|
| `optim-nb-1` | `simulation-table-{year}-optim-nb-1` | the first optimisation pass | every mode |
| `optim-nb-2` | `simulation-table-{year}-optim-nb-2` | the second pass | every mode that runs it (§ *Skipped stages* below) |
| `remix-hydro` | `simulation-table-{year}-remix-hydro` | `RemixHydroPostProcessCmd` (shave-peaks) | economy (both post-process lists) and adequacy |
| `adq-patch-csr` | `simulation-table-{year}-adq-patch-csr` | `UpdateMrgPriceAfterCSRcmd`, i.e. once the whole CSR treatment has been applied | economy with `include-adq-patch = true` |

Stage names are the file-name suffixes, declared once as constants on `IO::Outputs::OptimisationsSimulationTable`; they are part of the public output contract. The tables of one Monte-Carlo year live in a `std::map<std::string, SimulationTable>` created on demand (`tableForStage`) — `std::map` because its nodes are address-stable, so a pointer handed to the optim-1 call site stays valid once later stages are added, and because `SimulationTable` is move-only.

### Design: republish the results through the address tables

Building a simulation table needs a lot of context — the modeler `ILinearProblem` and its `OptimEntityContainer`, `ModelerData`, the `FillContext`, and the whole `AddLegacyExtraOutputs` derivation. Re-deriving any of that per stage was not an option, so the dump reuses `FillLegacySimulationTable` / `AddLegacyExtraOutputs` **verbatim** and only changes what they read.

The hinge is a table the solver already maintains. `PROBLEME_ANTARES_A_RESOUDRE::AdresseOuPlacerLaValeurDesVariablesOptimisees[i]` points at the exact result slot of variable `i` (`ValeursDeNTC[pdt].ValeurDuFlux[interco]`, `ResultatsHoraires[pays].TurbinageHoraire[pdt]`, ... — set in `opt_gestion_des_bornes_cas_lineaire.cpp`), and `OPT_AppelDuSimplexe` publishes the solution with `*address = X[i]`. Post-processes mutate those result structures **in place** and never write back into `X`, so a plain fill after a post-process would re-emit the values the solver left. Reading the addresses back — `X[i] = *address` — republishes the post-processed state into the solution vector, and every existing formula then reads the right numbers with no change at all. The same holds for the duals through `AdresseOuPlacerLaValeurDesCoutsMarginaux`, which `UpdateMrgPriceAfterCSRcmd` overwrites in place; that is what makes `price` a post-CSR value.

`SolutionRefreshedFromResults` (`LegacySimulationTableSnapshot.cpp`) is the RAII wrapper doing this: it saves `X` and `CoutsMarginauxDesContraintes` on construction, refreshes them from the address tables, and restores both on destruction. The restore is what guarantees the feature is **observation-only** — nothing the simulation computes afterwards can see that a dump happened.

### Call sites

- **Economy / hybrid** — `DumpSimulationTablePostProcessCmd`, a regular post-process command inserted into `OptPostProcessList` and `AdqPatchPostProcessList` at the point whose name it carries. A nullable `IO::Outputs::OptimisationsSimulationTable*` is threaded through `interfacePostProcessList::create`; it is null when the run does not write simulation tables, and the command is then a no-op.
- **Adequacy** — this mode calls `RemixHydroForAllAreas` inline rather than through a post-process list, so `adequacy.cpp` calls `DumpSimulationTableStage` directly. The call sits **inside** the `if (state.simplexRunNeeded)` branch: on a week that is not solved, the state to publish would be the previous week's.

Both go through `Antares::Optimization::DumpSimulationTableStage(tables, stage, problemeHebdo)`, which derives the week's `FillContext` and block index and forwards to `DumpSimulationTableAfterPostProcess`. It lives in its own translation unit (`LegacySimulationTableStage.cpp`) because it needs `buildFillContext`, which pulls in the OR-Tools call chain that the `unit-tests-for-solver-optimisation` target — which compiles `LegacySimulationTableSnapshot.cpp` directly — must stay clear of.

### Modeler rows in a stage table

A stage table would otherwise carry only the legacy half of a hybrid study. To avoid that, the solved modeler problem is retained past the solve on `PROBLEME_HEBDO::lastSolvedModelerProblem` (a `SolvedModelerProblem`: the `ILinearProblem`, its `OptimEntityContainer`, and the objective value), and `DumpSimulationTableAfterPostProcess` re-emits the modeler component rows before the legacy ones. This is cheap — `OrtoolsMipVariable` only wraps a raw `MPVariable*`, and the `MPSolver` that owns the data is already retained for the whole week in `ProblemeAResoudre->ProblemesSpx`. It is only populated when simulation tables are enabled.

**Caveat: a stage table mixes two vintages.** Post-processing mutates the legacy result structures and cannot move a modeler variable, so in a hybrid study the legacy rows are *post*-treatment while the modeler component rows are the values the solver left. A `gen1` row in `remix-hydro` must not be read as "after remix" — it is identical to its `optim-nb-2` row by construction.

### Selecting which stages to write

Every stage costs a full table's worth of memory for the whole Monte-Carlo year and the time to write it, and a run that only wants to inspect one treatment has no use for the other three. The selection is a comma-separated list of stage names, `all` (the default) meaning every stage, and it can be set in either place:

```ini
# settings/generaldata.ini
[output]
simulation-table-stages = optim-nb-2, adq-patch-csr
```

```
antares-solver -i study --output=simulation-tables --simulation-table-stages=optim-nb-2,adq-patch-csr
```

**The command line wins over `generaldata.ini`**, including `--simulation-table-stages=all`, which is how a study that restricts the stages is put back to the full set for one run. The two are told apart by the raw command-line string being empty, not by the resolved set, since an empty *set* is what "every stage" means internally.

Both go through `OptimisationsSimulationTable::parseStageSelection`, so an unknown name fails the run either way, with the valid names listed. The error says which of the two sources it read, because they send the reader to different files. The command-line list is validated *before* the study is loaded, so a typo there costs nothing; the ini list can only be checked after the load, and is only parsed when it is the one actually being used — so a command-line selection is also the way past a study whose ini value is wrong.

`Data::Parameters` therefore holds both the raw ini string (`simulationTableStagesStr`, filled by `SGDIntLoadFamily_Output`) and the resolved set (`simulationTableStages`). The stage names live in the simulation-table library, which sits *above* `Antares::study` in the layer graph, so the study cannot validate them itself; `Application::readDataForTheStudy` is the one place that sees both. Selection is orthogonal to `--output`: it narrows the set of tables, it does not enable them.

The filter lives at a single choke point. `tableForStage` returns `nullptr` for a stage that was not selected, so the stage is never even created — `stages()` does not contain it and the writer therefore emits no file for it, without needing a rule of its own. Both dump call sites already had to tolerate a null table (the pointer is null when the run writes no simulation tables at all), so selection reuses that path; `DumpSimulationTableStage` skips on a null table exactly as it skips on a null `tables`.

Because the filter only decides *whether* a stage is filled, the selection is **purely subtractive**: a stage that is still asked for is filled exactly as it would have been in a full run. The refresh is restored by `SolutionRefreshedFromResults` whether or not it ran, so skipping a dump cannot perturb a later one. This is checked end to end — the stage files of a restricted run are byte-identical to the corresponding files of a full run.

### Skipped stages and other limits

- **Empty stages produce no file.** `LegacySimulationTablesWriter` skips a stage whose table has no rows. This is a behaviour change: before per-stage tables, a header-only `optim-nb-2` file was written even when the second pass never ran (Expansion / MIP); now no file appears at all.
- **Weekly simplex range only.** With `simplex-range = day` the week is solved in seven intervals that each rebuild the address table, so only the last day would be readable. The dump then no-ops with a one-time warning. That setting is deprecated — `parameters.cpp` logs an error for it — but the error does not stop the load, so the path is still reachable with `--force`.
- **CSR-only quantities are out of reach.** Values that exist only in the patch's own structures (`...CSR`, `DENS`, `DtgMrgCsr`) have no LP variable in the weekly problem and therefore no entry in the address tables; the refresh cannot see them.
- **Rows, not schema.** Stage tables use the same fill code as the optimisation-pass tables, so their column set and row conventions (§2) are identical; only the values differ.
