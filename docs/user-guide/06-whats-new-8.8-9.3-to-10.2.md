# What's new since v8.8 / v9.3 (upgrading to v10.2)

This page summarizes, in plain terms, what has changed for **users** since the last two long-lived
versions (v8.8.21 and v9.3.11), for people upgrading straight to v10.2. It intentionally leaves out
internal refactors, test additions, CI changes and code-quality cleanups — for the exhaustive,
PR-by-PR technical log see [CHANGELOG.md](../developer-guide/CHANGELOG.md). For input/output format
changes you need to act on, see the [Migration guides](../reference/migration-guides.md).

Pick the section that matches your current version — v9.3.x already received several of the fixes
below as backports, so its section is shorter.

## Coming from v8.8.21

### Major change: the legacy GUI is gone

Starting with v10, the old desktop GUI has been removed in favor of Antares Web. If your
workflow depends on the GUI, plan for this before upgrading — see the migration guides for the
replacement workflows.

### Hydro

- **Scenarized/hourly hydro pumping and generation** — previously fixed daily values, now scenarized and hourly.
- **Final hydro reservoir level** can now be specified directly.
- **Reworked hydro remix behavior**: overflow is now taken from the weekly optimization problem instead of being recomputed afterwards, improving results for min-gen constraints.
- **Multiple Hydro Rule Curves** : optional scenarized reservoir levels (min/max/avg) per Monte-Carlo year, instead of a single curve.
- Many correctness fixes: infeasibilities caused by hydro, monthly turbine errors, negative hydro inflows, hydro power constraint bounds, and more.

### Short-term storage

- Withdrawal efficiency, penalties on level/injection/withdrawal, and gradient penalties.
- Additional constraints on injection, withdrawal and netting, now scenarizable, including scenarized inflows.
- Dynamic (user-defined) storage groups.
- Multiple fixes: efficiency checks, memory leaks, cost computation.

### Thermal & renewables

- **Dynamic clusters groups**: group names for thermal and renewable clusters are no longer fixed by the code — users define their own.
- New output variable: partial min-gen for thermal clusters.
- Fix for thermal cluster profit computation.

### Reserves

- **New feature: reserves participation modeling** — clusters and storages can now contribute to reserve requirements, with a dedicated YAML input format.

### Binding constraints

- Constraints can now be checked/scoped per scenario year.
- Several segfault and undefined-behavior fixes when exporting or using binding constraints.

### Modeler (component-based modeling)

Introduced and significantly extended over this period: multiple objectives, investment variables and
constraints, hybrid studies (mixing legacy and modeler components), port connections, time-dependent
expressions, additional operators (`min`, `max`, `pow`, `ceil`, `floor`, `round`, `abs`, `dual`,
`reduced_cost`), extra outputs and Parquet-format exports, and integer investment variables for hybrid
studies. If you plan to adopt the Modeler, most of what exists today post-dates v8.8.

### Problem Generator

New CLI tool to export the optimization
problem without running a full simulation — useful for external tooling and debugging.

### Outputs & simulation tables

- Export by Monte-Carlo year instead of one global aggregated file.
- Parquet output support for both the legacy solver and the Modeler.
- Export of the original (unfeasible) problem when a simulation fails, to help diagnose infeasibilities.
- "Infeasibility analyzer" to help identify which constraints are responsible for an infeasible problem.
- Warning when the final hydro level falls outside rule curves.

### Removed / changed behavior (check before upgrading)

- Legacy GUI removed (see above).
- TS generator standalone binary removed.
- `refreshtimeseries` and related `generaldata.ini` options removed.
- Some previously-tolerated warnings during study loading are now hard errors — `--force` can bypass some of them.

## Coming from v9.3.11

Your version already includes backports of several v10.x fixes (thermal memory leak, inter-monthly
breakdown loading, hydro remix STS levels fix, digest fixes, CentOS7 issues). What's genuinely new
between v9.3.11 and v10.2.0:

### Major change: the legacy GUI is gone

See above — this is the biggest change in this range. Everything else below is smaller in comparison.

### Features

- **Multiple Hydro Rule Curves** : optional scenarized hydro reservoir levels (min/max/avg per MC year).
- **Reserves participation modeling**
- Export by Monte-Carlo year instead of one global aggregated output file.
- Export of the original problem when a simulation is unfeasible, to ease debugging.
- Warning when the final hydro level is outside rule curves.
- Investment with integer variables for hybrid (Modeler) studies, plus improved MILP handling and error messages for hybrid studies.
- "Drop constraint" support for constraints outside their time window.
- Allow `problem-generator` to load GEMS/Modeler studies.
- Modeler: new `round` and `abs` operators.

### Bugfixes worth knowing about

- Fixed bounds on fictitious load and max unsupplied energy in the adequacy patch.
- Fixed MPS export for integer variables.
- Fixed Modeler output path and simulation-week handling for hybrid studies.
- Fixed reserve symmetries and a case where reserves participation was never actually computed.

### Renames (check scripts/output parsing)

- Output variable `PositiveUnsuppliedEnergy` renamed to `UnsuppliedEnergy`, `NegativeUnsuppliedEnergy` renamed to `Spillage`.

For anything not covered here (documentation improvements, developer-facing changes, CI), consult the full
[CHANGELOG.md](../developer-guide/CHANGELOG.md).
