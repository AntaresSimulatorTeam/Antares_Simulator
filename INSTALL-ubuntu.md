# Antares Simulator CMake Build Instructions

[Environnement](#environment) | [CMake version](#cmake-version) |  [Environnement build install](#env-build-install) | [Python version](#python-version) | [Dependencies](#dependencies) | [Building](#building-antares-solution) | [Tests](#tests) | [Installer creation](#installer-creation)

## C/I status
| OS     | System librairies | Built in libraries |
|:-------|--------|------|
| Ubuntu  | [![Status][linux_system_svg]][linux_system_link] | [![Status][linux_deps_build_svg]][linux_deps_build_link] |

[linux_system_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(system%20libs)/badge.svg

[linux_system_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(system%20libs)"

[linux_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(deps.%20compilation)/badge.svg

[linux_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(deps.%20compilation)"

## [CMake version](#cmake-version)
CMake 3.x must be used.
On some OS it is not available by default on the system.

#### Using snap
```
sudo snap install cmake --classic
```
#### Compiling from sources

```
sudo apt install build-essential libssl-dev
wget https://github.com/Kitware/CMake/releases/download/v3.16.5/cmake-3.18.1.tar.gz
tar -zxvf cmake-3.18.1.tar.gz
cd cmake-3.18.1
./bootstrap
make 
sudo make install
```
Note:
> You can use a different version of CMake. Check CMake website for available version and change the ``wget`` url.

You can then tell Ubuntu that a new version of cmake should be used :
```
sudo update-alternatives --install /usr/bin/cmake cmake /usr/local/bin/cmake 1 --force
```
## [Environnement build install](#env-build-install)
```
sudo apt-get install build-essential
```

## [Python version](#python-version)
Python 3.x must be used.

```
sudo apt-get install python3 python3-pip
```
Required python modules can be installed with :
```
pip3 install -r src/src/tests/examples/requirements.txt
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
- by using a package manager.
- by using pre-compiled external libraries provided by [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.2.0)

#### apt-get command
```
sudo apt install uuid-dev libcurl4-openssl-dev libssl-dev libwxgtk3.0-gtk3-dev libboost-test-dev libboost-filesystem-dev libboost-regex-dev libboost-dev
```

### [Automatic librairies compilation from git](#git_compil)
[Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is used as a git submodule for automatic librairies compilation from git.

Dependency can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default). For a list of available option see [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

Some dependencies can't be installed with a package manager. They can be built at configure step with a cmake option  : `-DBUILD_not_system=ON` (`ON` by default).

#### Defining dependency install directory
When using multiple directories for antares development with multiple branches it can be useful to have a common dependency install directory.

Dependency install directory can be specified with `DEPS_INSTALL_DIR`. By default install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note :
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

> If the dependency install directory contains CURL, OPENSSL or wxWidgets pre-compiled libraries an additionnal option must be used at configure time `-DUSE_PRECOMPILED_EXT=ON`

### Pre-compiled libraries download : release version only
You can download pre-compiled antares-deps archive from [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v1.1.0). Only release version are available.

There are still some system librairies that must be installed :

```
sudo apt-get install libuuid1 uuid-dev libssh2-1 libssh2-1-dev libidn2-0 libidn2-dev libidn11 libidn11-dev gtk2.0 libb64-dev libjpeg-dev libtiff-dev libsecret-1-dev
```

## [Building Antares Solution](#build)

Antares source directory is named `[antares_src]` in all following commands.

### 1 - Update git submodule for dependency build

First you need to update git submodule for dependency build :
```
git submodule update --init [antares_src]/antares-deps
```

### 2 - CMake configure

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
|`BUILD_TESTING`| Enable test build (default `OFF`)|
|`BUILD_OUTPUT_TEST`| Enable test with output compare build (default `OFF`)|

#### Configure

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

## Ubuntu .deb (Experimental)
 ```
cd _build
cpack -G DEB .
```

## Linux .tar.gz
 ```
cd _build
cpack -G TGZ .
```
There are still some system librairies that must be installed if you want to use *ANTARES*:

```
sudo apt-get install libcurl4 libwxgtk3.0
```