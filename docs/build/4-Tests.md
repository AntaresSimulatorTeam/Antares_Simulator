# Tests

Tests compilation  can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default)

After build, tests can be run with ``ctest`` :

```
cd _build
ctest -C Release --output-on-failure
```
Note:
> Tests with output comparison must be enabled using the option `-DBUILD_OUTPUT_TEST=ON` (`OFF` by default)

All tests are associated to a label and multiple labels can be defined. You can choose which tests will be executed at ctest run.

This is the list of the available labels :

| Label     | Description |
|:-------|-----|
| `units`  | Units tests|
| `end-to-end`  | End to end tests with antares study creation|
| `short-examples`  | Short duration pytest with antares solver call and objective function result check|
| `medium-examples`  | Medium duration pytest with antares solver call and objective function result check|
| `long-examples`  | Long duration pytest with antares solver call and objective function result check|
| `short-output`  | Short duration pytest with antares solver call and simulation output comparison|
| `sirius`  | Sirius related pytest|
| `coin`  | coin related pytest|
| `ortools`  | OR-Tools related pytest|

Note :
> Use `ctest -N` to see all available tests

Here is an example for running only units tests:
```
ctest -C Release --output-on-failure -L units
```
Here is an example for running only sirius tests without OR-Tools used:
```
ctest -C Release --output-on-failure -L sirius -LE ortools
```
Here is an example for running only short sirius tests without OR-Tools used:
```
ctest -C Release --output-on-failure -R "short-examples.*sirius" -LE "ortools"
``` 
Note :
> In this case the regex is on name (`-R`) so only short-examples are executed.
For more information on `ctest` call see [documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
