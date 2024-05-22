---
hide:
  - toc
---

# Tests (user)

Tests compilation can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default). It is required only for unit-tests and end-to-end tests.

After build, tests can be run with ``ctest`` :

```
cd _build
ctest -C Release --output-on-failure
```

All tests are associated to a label and multiple labels can be defined. You can choose which tests will be executed at ctest run.  
Use `ctest -N` to see all available tests

> For more information on `ctest` call, see [the cmake documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## Running JSON tests
To run JSON tests, a [study batch](https://github.com/AntaresSimulatorTeam/SimTest/releases) is first required.

```bash
cd ${Antares_Simulator_ROOT}/src/tests/resources/batches
wget https://github.com/AntaresSimulatorTeam/SimTest/releases/download/v8.3.2/valid-defaillance-ubuntu-20.04.zip
unzip valid-defaillance-ubuntu-20.04.zip
cd ${Antares_Simulator_ROOT}/build
ctest -C Release --output-on-failure -R json
```
