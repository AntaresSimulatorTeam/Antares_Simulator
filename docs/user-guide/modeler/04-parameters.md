# Parameters

The modeler parameters are currently held in a **parameters.yml** file under the study root directory. 

## Solver parameters

### solver
- **Expected value:** one of the following (case-sensitive):
    - `sirius` (LP only)
    - `scip` (MIP only)
    - `coin`
    - `xpress`
    - `glpk`
    - `highs`
    - `pdlp` (LP only)
- **Required:** **no**
- **Default value:** `sirius`
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
- **Required:** yes
- **Usage:** first timestamp to include in the simulation horizon. Must be included in the definition of 
  [data-series](02-inputs.md#data-series) that are time-dependent.

### last-time-step
- **Expected value:** positive integer (0 accepted)
- **Required:** yes
- **Usage:** last timestamp to include in the simulation horizon. Must be included in the definition of 
  [data-series](02-inputs.md#data-series) that are time-dependent.

## Outputs

### no-output
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** whether to generate [output files](03-outputs.md) at the end of the simulation


## Full example
~~~yaml
solver: xpress
solver-logs: false
solver-parameters: THREADS 1
no-output: false
first-time-step: 0
last-time-step: 2
~~~
