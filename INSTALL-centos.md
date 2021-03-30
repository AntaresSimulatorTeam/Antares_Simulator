# Antares Simulator CMake Build Instructions

[Environnement](#environment) | [CMake version](#cmake-version) | [GCC version](#gcc-version) | [Python version](#python-version) | [Dependencies](#dependencies) | [Building](#building-antares-solution) | [Tests](#tests) | [Installer creation](#installer-creation)

## C/I status
| OS     | System librairies | Built in libraries |
|:-------|--------|------|
| Centos7  | [![Status][centos_system_svg]][centos_system_link] | [![Status][centos_deps_build_svg]][centos_deps_build_link] |

[centos_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(deps.%20compilation)/badge.svg

[centos_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(deps.%20compilation)"

[centos_system_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(system%20libs)/badge.svg

[centos_system_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(system%20libs)"
 
## [CMake version](#cmake-version)
CMake 3.x must be used.
```
sudo yum install epel-release
sudo yum install cmake3
```
Note:
> All ``cmake``  command must be replaced by ``cmake3``.

## [GCC version](#gcc-version)
By default, GCC version of Centos7 is 4.8.5.
The compilation of  *ANTARES* requires C++17 support.

You can use a more recent version of GCC by enabling `devtoolset-7` :
```
sudo yum install centos-release-scl
sudo yum install devtoolset-7
```

Before compiling *ANTARES* we must launch a new shell with `scl` tool :
```
scl enable devtoolset-7 bash
```

## [Python version](#python-version)
Python 3.x must be used.

```
sudo yum install python3 python3-pip
```

Required python modules can be installed with :
```
pip3 install -r src/tests/examples/requirements.txt
```

## [Dependencies](#deps)
 ANTARES depends on several mandatory libraries. 
 - [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
 - [OR-Tools](https://github.com/AntaresSimulatorTeam/or-tools/tree/rte_dev_sirius) (fork from [RTE](https://github.com/rte-france/or-tools/tree/rte_dev_sirius) based on official OR-Tools github)
 - [wxWidgets](https://github.com/wxWidgets/wxWidgets)
 (Only for the complete Antares Simulator solution with GUI)
 - Boost libraries : test process filesystem regex dll (Only for unit tests)

This section describes the install procedures for the third-party Open source libraries used by ANTARES.
The install procedure can be done
- by compiling the sources after cloning the official git repository
- by using a package manager
- by using pre-compiled external libraries provided by [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.2.0)

#### Yum commands
```
sudo yum install redhat-lsb-core curl-devel wxGTK3-devel boost-test boost-filesystem boost-regex boost-devel unzip
```
### [Automatic librairies compilation from git](#git_compil)
[Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is used as a git submodule for automatic librairies compilation from git.

All dependencies can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default).
For a list of available option see [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

Some dependencies can't be installed with a package manager.
They can be built at configure step with a cmake option  : `-DBUILD_not_system=ON` (`ON` by default).

#### Defining dependency install directory
It can be useful to have a common dependency install directory, when using multiple directories for antares development
with multiple branches.

The dependency-install-directory can be specified with `DEPS_INSTALL_DIR`.
By default the install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note :
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

> If the dependency install directory contains CURL, or wxWidgets pre-compiled libraries an additionnal option must be used at configure time `-DUSE_PRECOMPILED_EXT=ON`

### Pre-compiled libraries download : release version only
You can download pre-compiled antares-deps archive from [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.1.0). Only release version are available.

There are still some system libraries that must be installed :

```
sudo yum install libuuid-devel libidn2-devel gtk2-devel redhat-lsb-core
```

## [Building Antares Solution](#build)

Antares source directory is named `[antares_src]` in all following commands.

### 1 - Update git submodule for dependency build

First you need to update git submodule for dependency build :
```
git submodule update --init [antares_src]/antares-deps
```

### 2 - Enable `devtoolset-7`

```
scl enable devtoolset-7 bash
```

### 3 - CMake configure

Here is a list of available CMake configure option :

|Option | Description |
|:-------|-------|
|`CMAKE_BUILD_TYPE` | Define build type. Available values are `release` and `debug`  |
|`BUILD_UI`|Enable or disable Antares Simulator UI compilation (default `ON`)|
|`BUILD_not_system`|Enable build of external librairies not available on system package manager (default `ON`)|
|`BUILD_ALL`|Enable build of ALL external librairies (default `OFF`)|
|`DEPS_INSTALL_DIR`|Define dependencies libraries install directory|
|`USE_PRECOMPILED_EXT`| This option must be set if you use precompiled external librairies (default `OFF`)|
|`BUILD_TESTING`| Enable test build (default `OFF`)|
|`BUILD_OUTPUT_TEST`| Enable test with output compare build (default `OFF`)|


```
cmake3 -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
```

### 4 - Build
 ```
cmake3 --build _build --config release -j8
```
Note :
> Compilation can be done on several processor with ```-j``` option.

## [Tests](#tests)

Tests compilation  can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default)

After build, tests can be run with ``ctest`` :
 ```
cd _build
ctest3 -C Release --output-on-failure
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
ctest3 -C Release --output-on-failure -L units
```` 

Here is an example for running only sirius tests without OR-Tools used:
```
ctest3 -C Release --output-on-failure -L sirius -LE ortools
```` 

Here is an example for running only short sirius tests without OR-Tools used:
```
ctest3 -C Release --output-on-failure -R "short-examples.*sirius" -LE "ortools"
```` 
Note :
> In this case the regex is on name (`-R`) so only short-examples are executed.
For more information on `ctest` call see [documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## [Installer creation](#installer)
CPack can be used to create the installer after the build phase.

## RHEL .rpm (Experimental)
 ```
cd _build
cpack3 -G RPM .
```
Note :
> `rpm-build` must be installed for RPM creation :  `sudo yum install rpm-build `

## Linux .tar.gz
 ```
cd _build
cpack3 -G TGZ .
```

There are still some system libraries that must be installed if you want to use *ANTARES*:

```
sudo yum install epel-release
sudo yum install curl wxGTK3
```
