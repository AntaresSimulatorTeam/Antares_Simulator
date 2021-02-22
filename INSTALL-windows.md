# Antares Simulator CMake Build Instructions

[CMake version](#cmake-version) | [Python version](#python-version) | [Dependencies](#dependencies) | [Building](#building-antares-solution) | [Tests](#tests) | [Installer creation](#installer-creation)

## C/I status
| OS     | System librairies | Built in libraries |
|:-------|--------|------|
| Windows  | [![Status][windows_vcpkg_svg]][windows_vcpkg_link] | [![Status][windows_deps_build_svg]][windows_deps_build_link] |

[windows_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(deps.%20compilation)/badge.svg

[windows_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(deps.%20compilation)"

[windows_vcpkg_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG)/badge.svg

[windows_vcpkg_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG)"

## [CMake version](#cmake-version)
CMake 3.x must be used. 

You can download latest Windows version directly from [CMake website](https://cmake.org/download/).

## [Python version](#python-version)
Python 3.x must be used.

You can download latest Windows version directly from [Python website](https://www.python.org/downloads/windows/).

Required python modules can be installed with :
```
pip install -r src/tests/examples/requirements.txt
```

## [Dependencies](#deps)
 ANTARES depends on severals mandatory libraries. 
 - [OpenSSL](https://github.com/openssl/openssl)
 - [CURL](https://github.com/curl/curl)
 - [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
 - [OR-Tools](https://github.com/AntaresSimulatorTeam/or-tools/tree/rte_dev_sirius) (fork from [RTE](https://github.com/rte-france/or-tools/tree/rte_dev_sirius) based on official OR-Tools github)
 - [wxWidgets](https://github.com/wxWidgets/wxWidgets)
 (Only for the complete Antares Simulator solution with GUI)
 - Boost librairies : test process filesystem regex dll (Only for unit tests)

This section describes the install procedures for the third-party Open source libraries used by ANTARES.
The install procedure can be done
- by compiling the sources after cloning the official git repository
- by using VCPKG
- by using pre-compiled external libraries provided by [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.2.0)
- 
### VCPKG

For Windows we will use [vcpkg](https://github.com/microsoft/vcpkg) to download and compile the librairies.

You must install the corresponding [vcpkg-triplet](https://vcpkg.readthedocs.io/en/latest/users/integration/#triplet-selection) depending on Antares version and libraries load:

- ``x64-windows``        : 64 bits version with dynamic librairies load
- ``x64-windows-static`` : 64 bits version with static librairies load

The vcpkg-triplet used will be named [vcpg-triplet] later in this document.

#### 1 Install vcpkg 

vcpkg can be installed anywhere on your computer :

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Note :
> all vcpkg further described commands must be run from the vcpkg folder. This folder will be named [vcpkg_root] later in this document.


#### 2 Install dependencies
```
cd [vcpkg_root]
vcpkg install openssl:[vcpg-triplet] 
vcpkg install curl:[vcpg-triplet] 
vcpkg install wxwidgets:[vcpg-triplet] 
vcpkg install boost-test:[vcpg-triplet]
vcpkg install boost-filesystem:[vcpg-triplet]
vcpkg install boost-process[vcpg-triplet]
vcpkg install boost-dll:[vcpg-triplet]
vcpkg install boost-regex:[vcpg-triplet]
```
### [Automatic librairies compilation from git](#git_compil)
[Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is used as a git submodule for automatic librairies compilation from git.

ALL dependency can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default). For a list of available option see [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

Some dependencies can't be installed with a package manager. They can be built at configure step with a cmake option  : `-DBUILD_not_system=ON` (`ON` by default).

#### Defining dependency install directory
When using multiple directories for antares development with multiple branches it can be useful to have a common dependency install directory.

Dependency install directory can be specified with `DEPS_INSTALL_DIR`. By default install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note :
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

> If the dependency install directory contains CURL, OPENSSL or wxWidgets pre-compiled libraries an additionnal option must be used at configure time `-DUSE_PRECOMPILED_EXT=ON`

### Pre-compiled libraries download : release version only
You can download pre-compiled antares-deps archive from [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.1.0). Only release version are available. 

Note:
> You must you use a MSVC version compatible with MSVC version used in GitHub Action.

## [Building Antares Solution](#build)

Antares source directory is named `[antares_src]` in all following commands.

### 1 - Update git submodule for dependency build

First you need to update git submodule for dependency build :
```
git submodule update --init [antares_src]/antares-deps
```

### 2 - CMake configure

Note :
> cpack NSIS installer creation need an 'out of source build'. The build directory must be outside `[antares_src]` directory

#### Configure options

Here is a list of available CMake configure option :

|Option | Description |
|:-------|-------|
|`CMAKE_BUILD_TYPE` | Define build type. Available values are `release` and `debug`  |
|`BUILD_UI`|Enable or disable Antares Simulator UI compilation (default `ON`)|
|`BUILD_not_system`|Enable build of external librairies not available on system package manager (default `ON`)|
|`BUILD_ALL`|Enable build of ALL external librairies (default `OFF`)|
|`DEPS_INSTALL_DIR`|Define dependencies libraries install directory|
|`USE_PRECOMPILED_EXT`| This option must be set if you use precompiled external librairies (default `OFF`)|
|`VCPKG_ROOT`| Define [vcpkg_root] |
|`VCPKG_TARGET_TRIPLET`| Define [vcpkg-triplet] |
|`BUILD_TESTING`| Enable test build (default `OFF`)|
|`BUILD_OUTPUT_TEST`| Enable test with output compare build (default `OFF`)|


#### Configure using vcpkg (recommended)

```
cmake -B _build -S [antares_src] -DVCPKG_ROOT=[vcpkg_root] -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] -DCMAKE_BUILD_TYPE=release
```


#### Configure withouth VCPKG

```
cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
```

### 3 - Build
 ```
cmake --build _build --config release -j8
```
Note :
> Compilation can be done on several processor with ```-j``` option.

## [Tests](#tests)

Tests compilation  can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default)

After build, tests can be run with ``ctest`` :
 ```
cd _build
ctest -C Release --output-on-failure
```
Note:
> Tests with output comparison must be enabled using the option `-DBUILD_OUTPUT_TEST=ON` (`OFF` by default)

All tests are associated to a label and multiple labels can be defined. You can choose which tests will be executed at ctest run.

This is the list of the availables labels :

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
```` 

Here is an example for running only sirius tests without OR-Tools used:
```
ctest -C Release --output-on-failure -L sirius -LE ortools
```` 

Here is an example for running only short sirius tests without OR-Tools used:
```
ctest -C Release --output-on-failure -R "short-examples.*sirius" -LE "ortools"
```` 
Note :
> In this case the regex is on name (`-R`) so only short-examples are executed.
For more information on `ctest` call see [documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## [Installer creation](#installer)
CPack can be used to create the installer after the build phase.

```
cd _build
cpack -GNSIS
```
Currently missing in NSIS installer :
- External librairies sources
