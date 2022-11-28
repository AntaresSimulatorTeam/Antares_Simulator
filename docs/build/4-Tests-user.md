# Tests (user)

Tests compilation can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default). It is required only for unit-tests and end-to-end tests.

After build, tests can be run with ``ctest`` :

```
cd _build
ctest -C Release --output-on-failure
```

All tests are associated to a label and multiple labels can be defined. You can choose which tests will be executed at ctest run.

This is the list of the available labels :

| Label     | Description |
|:-------|-----|
| `units`  | Units tests |
| `end-to-end`  | End to end tests with antares study creation |
| `json` | Run examples configured with a .json file (see below) |

Note :
> Use `ctest -N` to see all available tests

Note :
> In this case the regex is on name (`-R`) so only short-examples are executed.
For more information on `ctest` call see [documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

# Running JSON tests
To run JSON tests, a [study batch](https://github.com/AntaresSimulatorTeam/SimTest/releases) is first required.

```bash
cd ${Antares_Simulator_ROOT}/src/tests/resources/batches
wget https://github.com/AntaresSimulatorTeam/SimTest/releases/download/v8.3.2/valid-defaillance-ubuntu-20.04.zip
unzip valid-defaillance-ubuntu-20.04.zip
cd ${Antares_Simulator_ROOT}/build
ctest -C Release --output-on-failure -R json
```
