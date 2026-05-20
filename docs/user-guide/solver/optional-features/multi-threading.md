---
hide:
 - toc
---

# Multi-threading

## Week-level parallelism (intra-year)

_Available since v9.3.11_

### Overview

In addition to the existing MC-year-level parallelism described below, Antares Solver
can solve the 52 weekly subproblems **within a single Monte Carlo year** concurrently.
This forms a 2D concurrency model: MC years run in parallel across year-threads, and
within each year-thread the weekly LP solves can also run in parallel.

### When to use it

Week-level parallelism is most beneficial when:

- The number of MC years is **low relative to available CPU cores** (e.g. 4 years on a
  16-core machine leaves 12 cores idle with year-only parallelism).
- Individual weekly LP solves are **computationally heavy** (large networks, many
  constraints).
- The simulation uses **Fast Mode UC** (`unit-commitment-mode = fast`). Weekly
  parallelism is designed for Fast Mode because there are no cross-week
  unit-commitment dependencies in that mode.

The two parallelism levels compose freely: running 4 MC years in parallel and
4 weeks in parallel gives 16 concurrent LP solves.

### Configuration

#### Command-line flag

| Flag | Type | Default | Description |
|------|------|---------|-------------|
| `--parallel-week-count=N` | integer | `1` | Number of weeks to solve concurrently within each MC year. `1` disables week parallelism (sequential behaviour, identical to previous versions). Requires Fast Mode UC for guaranteed identical results. |

Example:

```bash
antares-solver --parallel --force-parallel=4 --parallel-week-count=4 -i /path/to/study
```

#### Combining with year-level parallelism

The total number of simultaneous LP threads equals:

```
nb_mc_years_in_parallel × nb_weeks_in_parallel
```

Size this product to match the available hardware cores to avoid over-subscription.
The solver emits a warning at startup if the product exceeds the detected hardware
concurrency.

**Example — 16-core machine:**

| `--force-parallel` | `--parallel-week-count` | Total threads |
|:------------------:|:-----------------------:|:-------------:|
| 16 | 1 | 16 (year-only) |
| 4 | 4 | 16 (2D) |
| 2 | 8 | 16 (2D) |
| 1 | 16 | 16 (week-only) |

### Resource usage

Each concurrent week solve operates on a **deep copy** of the week's
`PROBLEME_HEBDO` data structure, so memory usage scales linearly with the number
of concurrent week solves. On a study with large networks, monitor RSS when
increasing `--parallel-week-count`.

### Known limitations

- Designed for **Fast Mode UC** only. With Accurate or Full Integer UC there are
  cross-week unit-commitment constraints (minimum up/down times spanning week
  boundaries); using week parallelism in those modes may produce results that differ
  from the sequential solve. The solver issues a warning but does not abort.
- **Hydro approximation.** The initial reservoir level for each week is precomputed
  from the ventilation-provided target levels before the parallel fan-out. For areas
  where `TurbinageEntreBornes = true`, this introduces a bounded approximation error
  (the exact level depends on the LP result of the previous week). The error is
  acceptable in Fast Mode; it may accumulate in Accurate Mode.
- Week-level parallelism is compiled in by default (`ENABLE_WEEK_PARALLELISM=ON`).
  It can be disabled at CMake time with `-DENABLE_WEEK_PARALLELISM=OFF`, in which case
  the `--parallel-week-count` flag is accepted but silently treated as `1`.

---

[//]: # (TODO: update this page if needed)
_**This section is under construction**_

Multi-threading is also available on the proper calculation side, on a user-defined basis.

Provided that hardware resources are large enough, this mode may reduce significantly the overall runtime of heavy simulations.

To benefit from multi-threading, the simulation must be run in the following context:

- The [parallel](../02-command-line.md#simulation) option must be enabled (it is disabled by default)
- The simulation [mode](../static-modeler/04-parameters.md#mode) must be either `Adequacy` or `Economy`

When the "parallel" solver option is used, each Monte-Carlo year is dispatched in an individual process on the available CPU cores.
The number of such individual processes depends on the characteristics of the local hardware and on the value given to
the study-dependent [number-of-cores-mode](../static-modeler/04-parameters.md#number-of-cores-mode) advanced parameter.
This parameter can take five different values (Minimum, Low, Medium, High, Maximum).
The number of independent processes resulting from the combination (local hardware + study settings) is given in the
following table, which shows the CPU allowances granted in the different configurations.

| _Minimum_ |   _Low_   | _Medium_  |   _High_   | _Maximum_ |
|:---------:|:---------:|:---------:|:----------:|:---------:|
|     1     | Ceil(S/4) | Ceil(S/2) | Ceil(3S/4) |     S     |

## Formula for CPU cores

Starting from 9.2 we changed the formula for the number of cores to simplify. Here's the old values and the new ones.

### Starting from 9.2

-| _Available CPU Cores_ | _Minimum_ |   _Low_   | _Medium_  |   _High_   | _Maximum_ |
-|:---------------------:|:---------:|:---------:|:---------:|:----------:|:---------:|
-|          _1_          |     1     |     1     |     1     |     1      |     1     |
-|          _2_          |     1     |     1     |     1     |     2      |     2     |
-|          _3_          |     1     |     1     |     2     |     3      |     3     |
-|          _4_          |     1     |     1     |     2     |     3      |     4     |
-|          _5_          |     1     |     2     |     3     |     4      |     5     |
-|          _6_          |     1     |     2     |     3     |     5      |     6     |
-|          _7_          |     1     |     2     |     4     |     6      |     7     |
-|          _8_          |     1     |     2     |     4     |     6      |     8     |
-|          _9_          |     1     |     3     |     5     |     7      |     9     |
-|         _10_          |     1     |     3     |     5     |     8      |    10     |
-|         _11_          |     1     |     3     |     6     |     9      |    11     |
-|         _12_          |     1     |     3     |     6     |     9      |    12     |
-|      _S &gt; 12_      |     1     | Ceil(S/4) | Ceil(S/2) | Ceil(3S/4) |     S     |

### Before 9.2

-| _Available CPU Cores_ | _Minimum_ |   _Low_   | _Medium_  |   _High_   | _Maximum_ |
-|:---------------------:|:---------:|:---------:|:---------:|:----------:|:---------:|
-|          _1_          |     1     |     1     |     1     |     1      |     1     |
-|          _2_          |     1     |     1     |     1     |     2      |     2     |
-|          _3_          |     1     |     2     |     2     |     2      |     3     |
-|          _4_          |     1     |     2     |     2     |     3      |     4     |
-|          _5_          |     1     |     2     |     3     |     4      |     5     |
-|          _6_          |     1     |     2     |     3     |     4      |     6     |
-|          _7_          |     1     |     2     |     3     |     5      |     7     |
-|          _8_          |     1     |     2     |     4     |     6      |     8     |
-|          _9_          |     1     |     3     |     5     |     7      |     8     |
-|         _10_          |     1     |     3     |     5     |     8      |     9     |
-|         _11_          |     1     |     3     |     6     |     8      |    10     |
-|         _12_          |     1     |     3     |     6     |     9      |    11     |
-|      _S &gt; 12_      |     1     | Ceil(S/4) | Ceil(S/2) | Ceil(3S/4) |    S-1    |


[^23]: When the number of MC years to run is smaller than the allowance, the parallel run includes all of these years in a single bundle and there is no "reduced allowance" message

[^24]:
The smallest bundle in this case is the ninth (year number 97 to year number 100).The first 8 bundles involve 12 MC years each.
