---
hide:
 - toc
---

# Command-line instructions

**Executable**: antares-solver

## Simulation

| command                | usage                                                                                                                              |
|:-----------------------|:-----------------------------------------------------------------------------------------------------------------------------------|
| -i, --input            | Study folder                                                                                                                       |
| --expansion            | Force the simulation in [expansion](04-parameters.md#mode) mode                                                                    |
| --economy              | Force the simulation in [economy](04-parameters.md#mode) mode                                                                      |
| --adequacy             | Force the simulation in [adequacy](04-parameters.md#mode) mode                                                                     |
| --parallel             | Enable [parallel](optional-features/multi-threading.md) computation of MC years                                                    |
| --force-parallel=VALUE | Override the max number of years computed [simultaneously](optional-features/multi-threading.md)                                   |
| --use-ortools          | Use the [OR-Tools](https://developers.google.com/optimization) modelling library (under the hood)                                  |
| --ortools-solver=VALUE | The solver to use (only available if use-ortools is activated). Possible values are: `sirius` (default), `coin`, `xpress`, `scip`  |

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

| command                  | usage                                                                                                              |
|:-------------------------|:-------------------------------------------------------------------------------------------------------------------|
| --optimization-range     | Force the [simplex optimization range](04-parameters.md#simplex-range) ('day' or 'week')                           |
| --no-constraints         | Ignore all binding constraints                                                                                     |
| --no-ts-import           | Do not import timeseries into the input folder (this option may be useful for running old studies without upgrade) |
| -m, --mps-export         | Export anonymous MPS, weekly or daily optimal UC+dispatch linear                                                   |
| -s, --named-mps-problems | Export named MPS, weekly or daily optimal UC+dispatch linear                                                       |
| --solver-logs            | Print solver logs                                                                                                  |

## Misc.

| command         | usage                                                            |
|:----------------|:-----------------------------------------------------------------|
| --progress      | Display the progress of each task                                |
| -p, --pid=VALUE | Specify the file where to write the process ID                   |
| --list-solvers  | Display a list of LP solvers available through OR-Tools and exit |
| -v, --version   | Print the version of the solver and exit                         |
| -h, --help      | Display this help and exit                                       |