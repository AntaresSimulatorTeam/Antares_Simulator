---
hide:
  - toc
---

# Command-line instructions

**Executable**: antares-solver

## Simulation

| command                             | usage                                                                                                                                                                                               |
|:------------------------------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| -i, --input                         | Study folder                                                                                                                                                                                        |
| --expansion                         | Force the simulation in [expansion](04-parameters.md#mode) mode                                                                                                                                     |
| --economy                           | Force the simulation in [economy](04-parameters.md#mode) mode                                                                                                                                       |
| --adequacy                          | Force the simulation in [adequacy](04-parameters.md#mode) mode                                                                                                                                      |
| --parallel                          | Enable [parallel](optional-features/multi-threading.md) computation of MC years                                                                                                                     |
| --force-parallel=VALUE              | Override the max number of years computed [simultaneously](optional-features/multi-threading.md)                                                                                                    |
| --linear-solver=VALUE               | The optimization solver to use for linear problems. Possible values are: `sirius` (default, LP only), `coin`, `xpress`, `scip` (MIP only), `glpk` (Linux only), `highs`, `pdlp` (LP only), `gurobi` |
| --linear-solver-param=VALUE         | Linear solver-specific parameters. Syntax is solver-dependent, and only supported for `scip` and `xpress`                                                                                           |
| --linear-solver-param-optim-1=VALUE | Linear solver-specific parameters for first optimization. Only supported for `scip` and `xpress`.                                                                                                   |
| --linear-solver-param-optim-2=VALUE | Linear solver-specific parameters for second optimization. Only supported for `scip` and `xpress`.                                                                                                  |
| --use-optim-1-basis-next-week       | Use basis of first optimization in next week's first optimization                                                                                                                                   |
| --use-optim-1-basis-optim-2         | Use basis of first optimization in second optimization                                                                                                                                              |
| --quadratic-solver=VALUE            | The optimization solver to use for quadratic problems. Possible values are: `sirius` (default), `pdlp`, `xpress`, `gurobi`                                                                          |
| --quadratic-solver-param=VALUE      | Quadratic solver-specific parameters. Syntax is solver-dependent.                                                                                                                                   |

## Parameters

| command                  | usage                                                                                             |
|:-------------------------|:--------------------------------------------------------------------------------------------------|
| -n, --name=VALUE         | Set the name of the new simulation                                                                |
| -g, --generators-only    | Run the time-series generators only                                                               |
| -c, --comment-file=VALUE | Specify the file to copy as comments of the simulation                                            |
| -f, --force              | Ignore all warnings at loading                                                                    |
| --no-output              | Do not write the results in the output folder                                                     |
| -y, --year=VALUE         | Override the [number of MC years](04-parameters.md#nbyears)                                       |
| --year-by-year           | Force the [writing the result output for each year](04-parameters.md#year-by-year) (economy only) |
| --derated                | Force the [derated](04-parameters.md#derated) mode                                                |
| -z, --zip-output         | Write the results into a single zip archive                                                       |

## Optimization

| command                  | usage                                                                                                                                                                                                                                                                                     |
|:-------------------------|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| --optimization-range     | Force the [simplex optimization range](04-parameters.md#simplex-range) ('day' or 'week')                                                                                                                                                                                                  |
| --no-constraints         | Ignore all binding constraints                                                                                                                                                                                                                                                            |
| --no-ts-import           | Do not import timeseries into the input folder (this option may be useful for running old studies without upgrade)                                                                                                                                                                        |
| -m, --mps-export         | Export anonymous MPS, weekly or daily optimal UC+dispatch linear (MPS will be named if the problem is infeasible)                                                                                                                                                                         |
| -s, --named-mps-problems | Export named MPS, weekly or daily optimal UC+dispatch linear                                                                                                                                                                                                                              |
| --solver-logs            | Print solver logs                                                                                                                                                                                                                                                                         |
| --linear-solver-param    | Set solver-specific parameters for linear problems, for instance `--linear-solver-param="THREADS 1 PRESOLVE 1"` for XPRESS or `--linear-solver-parameters="parallel/maxnthreads 1, lp/presolving TRUE"` for SCIP. Syntax is solver-dependent, and only supported for SCIP, XPRESS & PDLP. |
| --quadratic-solver-param | Set solver-specific parameters for quadratic problems.                                                                                                                                                                                                                                    |

## Misc.

| command         | usage                                                            |
|:----------------|:-----------------------------------------------------------------|
| --progress      | Display the progress of each task                                |
| -p, --pid=VALUE | Specify the file where to write the process ID                   |
| -l, --list-solvers  | Display a list of LP solvers available through OR-Tools and exit |
| -v, --version   | Print the version of the solver and exit                         |
| -h, --help      | Display this help and exit                                       |

### Deprecated options

* `--solver` (alias for `--linear-solver`) is deprecated.
* `--solver-parameters` (alias for `--linear-solver-param`) is deprecated.
