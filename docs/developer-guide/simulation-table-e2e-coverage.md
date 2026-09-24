# Simulation table — end-to-end coverage

Antares currently produces two output systems in parallel:

* the historical **`mc-all` / `mc-ind`** text tree, built by the
  `src/solver/variable/` template machinery — this is the production output;
* the flat **simulation table**
  (`simulation-table-<year>-optim-nb-<n>.csv` / `.parquet`), written only when
  the solver runs with `--output=all` or `--output=simulation-tables`.

The simulation table is not used in production yet. This page tracks the e2e
coverage that has to be in place before it can be.

## Test assets

| Layer | Location |
|---|---|
| C++ unit tests | `src/tests/src/io/outputs/testSimulationTable.cpp`, `test-parquet-simulation-table-writer.cpp` |
| Worked-example e2e (hand-picked hours, inline expected values) | `src/tests/cucumber/features/solver-features/legacy_simulation_table.feature` |
| **Systematic legacy ⇔ table equivalence** | `src/tests/cucumber/features/solver-features/legacy_simulation_table_equivalence.feature` + `features/steps/common_steps/simulation_table_equivalence.py` |
| Parquet round-trip (pure modeler) | `src/tests/cucumber/features/modeler-features/test_launcher_parquet.feature` |
| Folder diff vs inline reference | `hybrid_studies.feature` (`simulation tables match the references`) |

## The equivalence check

`the simulation table matches the legacy mc-ind output for year N` walks a
declarative mapping over
every area / thermal cluster / short-term-storage cluster / link of the study
and asserts, for each timestep the table covers, that the table value equals its
`mc-ind` counterpart within a per-quantity tolerance.

Design points:

* reads the **final optimisation pass** — `optim-nb-2` when present, else
  `optim-nb-1`. `optim-nb-1` is the LP relaxation and diverges from `mc-ind` on
  unit-commitment studies;
* `mc-ind` folders are 1-based, the table's `scenario_index` is 0-based
  (`year N` ⇒ `mc-ind/0000N` ⇔ `simulation-table-(N-1)-...`);
* no frozen reference file — it compares two live outputs, so it keeps working
  while the table layout/naming still changes;
* `basis_status` is never compared (see CHANGELOG `#3233`);
* tolerances are generous because `mc-ind` hourly columns are printed rounded;
  the table carries full precision.

## Quantity coverage

| Quantity | Table `output` | mc-ind source | Status |
|---|---|---|---|
| Unsupplied energy | `unsupplied_energy` | area `values-hourly` `UNSP. ENRG` | ✅ equivalence-checked |
| Spilled energy | `spilled_energy` | area `values-hourly` `SPIL. ENRG` | ✅ equivalence-checked |
| Area marginal price | `price` | area `values-hourly` `MRG. PRICE` | ✅ equivalence-checked — dual-derived, see gaps |
| Actual load | `actual_load` | area `values-hourly` `LOAD` | ✅ equivalence-checked |
| Thermal generation | `generation_power` | area `details-hourly` `<cluster>` / `MWh` | ✅ equivalence-checked (optim-nb-2) |
| Units on (ceil) | `actual_num_units_on` | area `details-hourly` `<cluster>` / `NODU` | ✅ equivalence-checked — skipped when `[other preferences] unit-commitment-mode` is `fast` (`Mapping.requires_accurate_uc`); the ST value is `ceil(x(NumberOfDispatchableUnits))` while mc-ind's NODU is the fast-UC heuristic's own unit count, two independently-computed integers that can differ by 1 on borderline hours |
| STS injection / withdrawal / level | `injection_power` / `withdrawal_power` / `level` | area `details-STstorage-hourly` | ✅ equivalence-checked — needs the STS cluster's `list.ini` section id to equal its `name` (mc-ind captions the display name, the ST component uses the id); true in all current fixtures but not enforced, see gaps |
| Link flow / abs / minus / loop | `flow` / `abs_flow` / `minus_flow` / `actual_loop_flow` | link `values-hourly` `FLOW LIN.` / `LOOP FLOW` | ✅ equivalence-checked |
| Raw `num_units_on` (LP value) | `num_units_on` | — | ⛔ not equivalence-checked — limitation: fractional LP value, no integer mc-ind column to compare to (`actual_num_units_on` is the checked, rounded counterpart) |
| Derived costs (`prop_cost`, `non_prop_cost`, `imbalance_cost`, …) | — | — | ⛔ not equivalence-checked — limitation: no mc-ind column at all (mc-ind never printed these); values also carry anti-degeneracy noise. Tested instead as hand-derived worked examples in `legacy_simulation_table.feature` |
| Emissions (`co2_emissions`, …) | `*_emissions` | area `values-hourly` `CO2 EMIS.` (area total only) | ⛔ not equivalence-checked — limitation: mc-ind only prints an area-level total, not per-cluster, so per-cluster ST rows have nothing to diff against. Tested instead as a worked example in `legacy_simulation_table.feature` |
| Hydro `level_percentage`, `actual_inflows`, `hydro_shadow_price`, `bellman_value` | — | `H. LEV` is absolute, not a direct match | ⛔ not equivalence-checked — limitation: `H. LEV` is a different (absolute, not percentage) quantity. Tested instead as a worked example in `legacy_simulation_table.feature` |
| Congestion fees (`abs_congestion_fee`, `alg_congestion_fee`) | `abs_congestion_fee` / `alg_congestion_fee` | link `values-hourly` `CONG. FEE (ABS./ALG.)` | ✅ equivalence-checked — dual-derived, see gaps |
| Reserve area spilled/unsupplied energy | `spilled_energy_reserve_<id>` / `unsupplied_energy_reserve_<id>` | area `values-hourly` `<reserve>_SPIL.` / `<reserve>_UNSP.` | ✅ equivalence-checked |
| Reserve thermal participation (on/off units) | `units_on_reserve_power_<id>` / `units_off_reserve_power_<id>` | area `details-hourly` `<reserve>_<cluster>` / `<reserve>_<cluster>_off` | ✅ equivalence-checked |
| Reserve costs (`reserve_imbalance_cost_<id>`, `reserve_participation_cost_<id>`) | — | — | ⛔ not equivalence-checked — limitation: no mc-ind column (same reason as the derived costs above) |
| Reserve total participation (`reserve_power_<id>`) | — | — | ⛔ not equivalence-checked — redundant, not a limitation: always equals `units_on_reserve_power` + `units_off_reserve_power`, both of which are already checked individually |
| STS/hydro reserve participation (`reserve_released_power_<id>`, `reserve_stored_power_<id>`) | — | area `details-STstorage-hourly` `<reserve>_<sts>` (mc-ind class exists) | ⏳ not equivalence-checked yet — not a limitation: an mc-ind counterpart exists, but no test study currently has STS/hydro reserve participation to exercise it. Good candidate for the next addition |

## Known gaps

* **MIP / MILP weeks** — dual-derived rows (`price`, STS `profit`,
  `hydro_shadow_price`) are zero on weeks solved as MIP because duals are not
  extracted there. See `docs/architecture/legacy-extra-outputs-spec-checklist.md`.
* **Adequacy patch** — `domestic_unsupplied_energy` (DENS),
  `is_local_matching_rule_violated` (LMR.VIOL) blocked on ANT-5240; CSR outputs
  out of scope.
* **Coverage is hybrid-only.** Pure-classic studies produce a sparse table
  (`SimulationTableWriter` throws on an empty table); pure-modeler has only the
  Parquet round-trip test.
* **No frozen-reference regression** in `src/tests/run-study-tests/` — deferred
  until the table file layout (`feature/st_stage_selection`: per-MC-year files,
  per-stage tables) stabilises.
* **STS mc-ind caption vs. ST component id.** `_sts_clusters()` in
  `simulation_table_equivalence.py` uses the cluster's `list.ini` section
  (its id) both to look up the mc-ind `details-STstorage-hourly` column and to
  build the ST component name. mc-ind actually captions the column with the
  cluster's `name` field, not its id, so the mapping silently produces zero
  comparisons on any study where a storage's id and name differ (e.g.
  `hybrid-legacy-equivalent/8_3`'s `battery ev` id / `Battery EV` name). All
  STS fixtures used by the equivalence suite today keep id == name, so this
  hasn't caused a false pass, but it should be fixed (resolve the display name
  for the mc-ind lookup) before relying on a study where they diverge.

## Adding coverage

* **New quantity, direct mc-ind counterpart** — add a `Mapping(...)` row to
  `LEGACY_TO_ST` in `simulation_table_equivalence.py`. Pick a study that
  exercises it, confirm the sign and the `mc-ind` print precision, set `atol` /
  `rtol` accordingly.
* **New study** — add a `Scenario` to
  `legacy_simulation_table_equivalence.feature` pointing at a study already in
  the `Antares_Simulator_Tests_NR` submodule; `--output=all` plus the existing
  `year-by-year` forcing in `init_simulation` is enough.
* **Quantity with no mc-ind counterpart** (derived costs, hydro %) — keep it as
  a worked example in `legacy_simulation_table.feature` with an inline expected
  value and a comment deriving it.
