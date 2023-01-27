# Antares dependencies build

 ANTARES Simulation and antaresXpansion depends on severals mandatory libraries.
 - [Sirius Solver](https://github.com/rte-france/sirius-solver)
 - [wxWidgets](https://github.com/wxWidgets/wxWidgets)
 (Only for the complete Antares Simulator solution with GUI)
 - Boost libraries: test (Only for unit tests)

This repository is used to compile these libraries from sources.

We can distinguish 2 kind of libraries:
- system libraries: wxWidgets, Boost
Theses libraries can be installed from a package manager (apt-get for debian, yum for centos and vcpkg for windows)
- solver libraries: Sirius Solver
Theses libraries can't be installed from a package manager. They must be compiled from sources.

This is also needed for coin checkout. Without git 2.x there are changes after git clone and there is an issue with CMake `ExternalProject_Add`

## Choose built libraries
You can choose built librairies with these options :

|Option | Description | Default |
|:-------|-------|-------|
|`BUILD_ALL`|Enable build of ALL external librairies| `OFF`|
|`BUILD_not_system`|Enable build of external librairies not available on system package manager | `ON`|
|`BUILD_sirius`|Build Sirius solver | `OFF` (`ON` if `BUILD_not_system`)|
|`BUILD_coin`|Build all COIN librairies | `OFF` (`ON` if `BUILD_not_system`)|
|`BUILD_CoinUtils`|Build CoinUtils | `OFF` (`ON` if `BUILD_coin`)|
|`BUILD_Osi`|Build Osi | `OFF` (`ON` if `BUILD_coin`)|
|`BUILD_Clp`|Build Clp | `OFF` (`ON` if `BUILD_coin`)|
|`BUILD_Cgl`|Build Cgl | `OFF` (`ON` if `BUILD_coin`)|
|`BUILD_Cbc`|Build Cbc | `OFF` (`ON` if `BUILD_coin`)|
|`BUILD_gflags`|Build gflags | `OFF` (`ON` if `BUILD_not_system`)|
|`BUILD_glog`|Build glob | `OFF` (`ON` if `BUILD_not_system`)|
|`BUILD_wxWidgets`|Build wxWidgets | `OFF`|
|`BUILD_BOOST`|Build Boost | `OFF`|

## CMake configure option

Here is a list of other available CMake configure option :

|Option | Description | Default |
|:-------|-------|-------|
|`CMAKE_BUILD_TYPE` |Define build type. Available values are `Release` and `Debug`  | `Release`|
|`DEPS_INSTALL_DIR`|Define libraries install directory| `<antares_deps_checkout_dir>/../rte-antares-deps-<build_type>`|
|`sirius_solver_REPO`| Define Sirius solver repository used| `https://github.com/rte-france/sirius-solver`|
|`sirius_solver_BRANCH`| Define Sirius solver branch used| `antares_integration`|

Note:
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

Note:
> Sirius solver configuration will be written in repository-config.txt
## Build
Build is done at CMake configure step :

`cmake -B _build -S . -DDEPS_INSTALL_DIR=<deps_install_dir>`

## Using pre-compiled Antares external libraries
When pre-compiled libraries are used to build Antares Simulator or antaresXpansion, some additionnal definitions are needed:
- wxWidgets

When running the CMake configure step for either Antares Simulator or antaresXpansion, an additionnal option is needed `-DUSE_PRECOMPILED_EXT=ON`.
The configuration step becomes

`cmake -B _build -S src -DDEPS_INSTALL_DIR=<deps_install_dir> -DUSE_PRECOMPILED_EXT=ON`
