# Study tests

The study tests run an executable on a study, check its results, restore any
modified study settings, and remove generated logs and outputs. Reference
outputs are preserved.

There are two entry points:

- `test_from_json.py` discovers studies in `resources/batches`, excluding those
  already covered by cucumber, and reads their runs and checks from
  `check-config.json`.
- `test_unfeasible_problem.py` keeps its checks, expected weeks, and skip markers
  in Python. Its executable and options come from execution-only entries in each
  study's `check-config.json`.

## Per-test configuration

Each `check-config.json` is an array. Each entry describes one run and must have
a unique `name`, an `exe`, and an `options` array. JSON-driven tests also require
`checks`.

```json
[
    {
        "name": "outputs_1node_ltstorage",
        "exe": "antares-solver",
        "options": [
            "--solver=sirius",
            "--output=simulation-tables",
            "--simulation-table-stages=optim-nb-1"
        ],
        "checks": {
            "simu_table_compare": null
        }
    }
]
```

`exe` is either `antares-solver` or `antares-problem-generator`, not a binary
path. `options` contains literal command-line arguments in order. Use `[]` for
no extra options. Flags, repeated options, and values containing spaces are
supported; do not add shell quoting around individual arguments.

The runner supplies the study argument automatically: `-i <study>` for
`antares-solver`, or a positional `<study>` for `antares-problem-generator`.
Do not include that argument in `options`.

No execution options are inferred from the study, its batch, or the check type.
For example, use `--solver=sirius`, `--solver=coin`,
`--named-mps-problems`, `--force-parallel=4`, or the simulation-table switches
explicitly when needed. To run a study in multiple ways, add separate named
entries with the corresponding checks and options.

The schema is `parse_studies/json_schema.json`. Missing files, malformed JSON,
invalid execution settings, and duplicate names are errors; they do not
silently select a default executable or drop a test.

### Infeasible-problem tests

Add execution-only entries to each study under
`resources/Antares_Simulator_Tests_NR/unfeasible-problem`. The entry's `name`
must exactly match its Python test function's name, without pytest's
`[study_path0]` suffix. For example, `unfeasible_problem_01/check-config.json`
needs these four entries:

```json
[
    {
        "name": "test_unfeasible_problem_01__error_verbose",
        "exe": "antares-solver",
        "options": ["--solver=sirius"]
    },
    {
        "name": "test_unfeasible_problem_01__error_dry",
        "exe": "antares-solver",
        "options": ["--solver=sirius"]
    },
    {
        "name": "test_unfeasible_problem_01__warning_verbose",
        "exe": "antares-solver",
        "options": ["--solver=sirius"]
    },
    {
        "name": "test_unfeasible_problem_01__warning_dry",
        "exe": "antares-solver",
        "options": ["--solver=sirius"]
    }
]
```

For studies `02` through `10`, provide the corresponding `warning_dry` and
`error_dry` entries. The tests for `09` and `10` retain their existing skip
markers. Infeasibility behavior and expected failing weeks remain defined in
Python, not in these execution-only entries.

## Executable paths

Configure CMake with `BUILD_TESTING=ON`. CMake generates
`src/tests/run-study-tests/executables.ini`, which is Git-ignored:

```ini
[executables]
antares-solver = /absolute/build/path/to/antares-solver
antares-problem-generator = /absolute/build/path/to/antares-problem-generator
```

Paths are generated from the actual CMake targets. The session-scoped `exe`
fixture reads both paths once for the pytest session. Only the executable
selected by a test needs to have been built.

As with cucumber's `behave.ini`, this file belongs to the last configured build.
For multi-configuration generators it selects `CMAKE_BUILD_TYPE`, or the first
configuration if that variable is unset. Configure again when switching builds
or configurations; `ctest -C` does not change this source-side mapping.

## Running

From `src/tests/run-study-tests`, using a Python environment containing the
packages in `requirements.txt`:

```bash
python -m pytest -m json
python -m pytest -m unfeasible
```

For PyCharm, create a Python configuration with **Module name** `pytest`,
**Parameters** `-m json`, and **Working directory**
`src/tests/run-study-tests`. Alternatively, use a pytest configuration targeting
`test_from_json.py` with additional arguments `-m json`.
The filename is `test_from_json.py`, not `test_from_json.pyscript`.

From the build directory:

```bash
ctest -C Release --output-on-failure -R '^json$'
ctest -C Release --output-on-failure -R '^unfeasible$'
```

CTest registers only `json` and `unfeasible` for these scripts. Former variant
names (`named-mps`, `antares-problem-generator-named-mps`, `milp-cbc`, `parallel`,
and `simulation-table-compare`) are removed. CI still downloads the same batches,
but runs each through `json` using the settings inside that batch.

The old pytest arguments `--antares-simu-path`, `--antares-problem-generator-exe`,
`--solver`, `--named-mps-problems`, `--force-parallel`, and
`--dump-simulation-tables` are no longer supported.

Runner regression tests use temporary data and mocked processes:

```bash
python -m pytest tests
```

### Dataset migration and rollout

Before deploying this runner and its CI changes, migrate the `check-config.json`
files in the published SimTest batches and in the
`Antares_Simulator_Tests_NR` submodule. Publish/select migrated batches and update
the submodule reference as a coordinated rollout. Those external changes are
not part of this repository refactor.

The new runner deliberately has no legacy-configuration fallback. Old datasets
will fail until migrated. Explicitly preserve the intended former variant's
options in every run entry, including `--solver=sirius` where previously supplied
by default. The former full global run matrix is not recreated automatically.

## Execution and cleanup

The collector passes the full JSON entry to the test. Each test constructs its
simulation using `exe` and `options`, creates its checks using that same
simulation, and calls `check_runner.run(simulation, checks)`.

`check_runner` no longer creates or selects a simulation. It registers the
checks for teardown, runs the supplied simulation, and then runs the checks.
Its fixture teardown restores settings changed by checks and removes generated
results, including when execution or a check fails. Return-code checks can
disable the runner's immediate failure exception to inspect expected failures.

## Adding a check

1. Implement a `check_interface` subclass in `check_on_results`, with `run()`
   and `name()` methods. Do not execute a simulation in the constructor.
2. Register its construction in `check_on_results/create_checks.py`.
3. Add its JSON grammar and allowed-check requirement to
   `parse_studies/json_schema.json`.
4. Add the check to the relevant `check-config.json` entries, with the explicit
   executable and options needed to produce the checked results.
