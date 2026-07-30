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
~~~
