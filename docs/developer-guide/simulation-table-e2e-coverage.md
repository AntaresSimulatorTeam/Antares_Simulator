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
declarative mapping (`LEGACY_TO_ST` in `simulation_table_equivalence.py`) over
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
| Unsupplied energy | `unsupplied_energy` | area `values-hourly` `UNSP. ENRG` | ✅ equivalence |
| Spilled energy | `spilled_energy` | area `values-hourly` `SPIL. ENRG` | ✅ equivalence |
| Area marginal price | `price` | area `values-hourly` `MRG. PRICE` | ✅ equivalence — dual-derived, see gaps |
| Actual load | `actual_load` | area `values-hourly` `LOAD` | ✅ equivalence |
| Thermal generation | `generation_power` | area `details-hourly` `<cluster>` / `MWh` | ✅ equivalence (optim-nb-2) |
| Units on (ceil) | `actual_num_units_on` | area `details-hourly` `<cluster>` / `NODU` | ✅ equivalence (non-fast UC only) |
| STS injection / withdrawal / level | `injection_power` / `withdrawal_power` / `level` | area `details-STstorage-hourly` | ✅ mapping in place (no study with STS clusters exercises it yet) |
| Link flow / abs / minus / loop | `flow` / `abs_flow` / `minus_flow` / `actual_loop_flow` | link `values-hourly` `FLOW LIN.` / `LOOP FLOW` | ✅ equivalence |
| Raw `num_units_on` (LP value) | `num_units_on` | — | not compared (fractional; `actual_num_units_on` is the mc-ind match) |
| Derived costs (`prop_cost`, `non_prop_cost`, `imbalance_cost`, …) | — | — | worked-example only (`legacy_simulation_table.feature`); carry anti-degeneracy noise |
| Emissions (`co2_emissions`, …) | `*_emissions` | area `values-hourly` `CO2 EMIS.` (area total only) | worked-example only; no per-cluster mc-ind column |
| Hydro `level_percentage`, `actual_inflows`, `hydro_shadow_price`, `bellman_value` | — | `H. LEV` is absolute, not a direct match | worked-example only |
| Congestion fees (`abs_congestion_fee`, `alg_congestion_fee`) | — | link `CONG. FEE (ABS./ALG.)` | candidate — validate sign/precision then add to `LEGACY_TO_ST` |

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
