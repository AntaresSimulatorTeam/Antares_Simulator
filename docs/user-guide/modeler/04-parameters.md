# Parameters

The modeler parameters are currently held in a **parameters.yml** file under the study root directory.

**Note:** In hybrid (solver + modeler) studies, `parameters.yml` is ignored. Use command-line options to set solver parameters instead.
This file is only used in pure modeler mode (`antares-modeler`). 

## Solver parameters

### solver
- **Expected value:** one of the following (case-sensitive):
    - `sirius` (LP only)
    - `scip` (MIP only)
    - `coin`
    - `xpress`
    - `glpk` (Linux only)
    - `highs`
    - `pdlp` (LP only)
    - `gurobi`
- **Required:** **yes**
- **Usage:** the solver to use for optimization problem resolution

### solver-logs
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** whether to activate solver output in the logs (useful for debugging)

### solver-parameters
- **Expected value:** string that must be comprehensible by the [OR-Tools-MPSolver](https://developers.google.com/optimization/lp/mpsolver) 
  implementation of the selected [solver](#solver)
- **Required:** no
- **Default value:** empty
- **Usage:** Set solver-specific parameters, for instance `THREADS 1 PRESOLVE 1` for XPRESS or 
  `parallel/maxnthreads 1, lp/presolving TRUE` for SCIP. Syntax is solver-dependent, and only supported for SCIP, XPRESS & PDLP.

## Horizon

### first-time-step
- **Expected value:** positive integer (0 accepted)
- **Required:** no
- **Default value:** `0`
- **Usage:** first timestamp to include in the simulation horizon. Must be included in the definition of 
  [data-series](02-inputs.md#data-series) that are time-dependent.

### last-time-step
- **Expected value:** positive integer (0 accepted)
- **Required:** no
- **Default value:** `167`
- **Usage:** last timestamp to include in the simulation horizon. Must be included in the definition of 
  [data-series](02-inputs.md#data-series) that are time-dependent.

## Scenarios

### scenario-scope

Selects which Monte-Carlo scenarios to simulate. Indices are 0-based, consistent with the
`modeler-scenariobuilder.dat` file convention.

The base scenario set is defined by **exactly one** of two mutually exclusive keys:
`include` (inline) or `playlist-file` (from a JSON file). `exclude` is optional and applies to
either form.

- **Required:** no
- **Default value:** runs scenario `0` only (if the key is absent or the block is empty)

**Inline form (`include` / `exclude`):**

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `include` | list | — | Scenarios to run (required in inline form) |
| `exclude` | list | — | Scenarios to remove from the base set (optional) |

Each entry in `include` or `exclude` may be:

- An integer: `5` → scenario 5
- A string integer: `"5"` → scenario 5 (identical to `5`)
- A range: `"0-9"` → scenarios 0 through 9 inclusive (10 scenarios)

**Playlist-file form:**

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `playlist-file` | string | — | Path to a JSON file whose array holds the base set of scenarios (each element being an integer, a string integer or a `"a-b"` range string) |

Rules:

- All indices must be ≥ 0.
- Overlapping entries in `include` are deduplicated automatically.
- Excludes that do not appear in the base set produce a warning and have no effect.
- Output is always sorted in ascending order.
- `exclude` cannot be used without `include` or `playlist-file`.

Examples:

```yaml
# Run a single scenario
scenario-scope:
  include:
    - 0
```

```yaml
# Run scenarios 0 to 99
scenario-scope:
  include:
    - "0-99"
```

```yaml
# Run scenarios 0–19 and 49–59, but skip 9 and 14
scenario-scope:
  include:
    - "0-19"
    - "49-59"
  exclude:
    - 9
    - 14
```

```yaml
# Base set read from a JSON file (e.g. playlist.json: [0, "2", "5-9"])
scenario-scope:
  playlist-file: playlist.json
```

## Outputs

### no-output

- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** whether to generate [output files](03-outputs.md) at the end of the simulation

### export-mps

- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** whether to generate mps and _structure_ files at the end of the simulation


## Full example
~~~yaml
solver: xpress
solver-logs: false
solver-parameters: THREADS 1
no-output: false
export-mps: false
first-time-step: 0
last-time-step: 2
scenario-scope:
  include:
    - "0-9"
    - 15
  exclude:
    - 3
~~~
