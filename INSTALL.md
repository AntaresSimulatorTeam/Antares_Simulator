# Antares Simulator CMake Build Instructions

[Environnement](#environment) | [Dependencies](#dependencies) | [Building](#building-antares-solution)

## C/I status
| OS     | System librairies | VCPKG | Built in libraries |
|:-------|-----|--------|------|
| Linux  | [![Status][linux_system_svg]][linux_system_link] |Not available  | [![Status][linux_deps_build_svg]][linux_deps_build_link] |
| Windows  | Not available|  [![Status][windows_vcpkg_svg]][windows_vcpkg_link] | [![Status][windows_deps_build_svg]][windows_deps_build_link] |


[linux_system_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(system%20libs)/badge.svg?branch=feature%2Fcmake_build_dependency_option

[linux_system_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(system%20libs)"

[linux_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(deps.%20compilation)/badge.svg?branch=feature%2Fcmake_build_dependency_option

[linux_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(deps.%20compilation)"

[windows_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(deps.%20compilation)/badge.svg?branch=feature%2Fcmake_build_dependency_option

[windows_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(deps.%20compilation)"

[windows_vcpkg_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG)/badge.svg?branch=feature%2Fcmake_build_dependency_option

[windows_vcpkg_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(VCPKG)"

Note : Linux with VCPKG is not available due to issue in Find_wxWidgets.cmake with VCPKG use in linux.

## [Environment](#env)
ANTARES Solver/Simulator is a cross-platform project using components compatible
with many 32 bits and 64 bits OS (Windows, Linux, Unix). 

The source distribution accompanying this file has been tested for successful build on:

- Windows 7
- Windows 10
- Linux CentOS  6
- RHEL 7.3
- Ubuntu 18.04

## Note on End-Of-Line convention
If the origin of the source folder accompanying this file is a check-out 
from a Git repository, end-of-line should be in the style of the local OS

If the source folder was distributed along with the binary form of the code
within an Antares_Simulator installer package, the end-of-line style is that of
the OS the package is designed for

To build on Windows system, please change end-of-line style to appropriate standard 
Note that Visual Studio may carry out auto-reformating. 
 

## [Dependencies](#deps)
 ANTARES depends on severals mandatory libraries. 
 - [OpenSSL](https://github.com/openssl/openssl)
 - [CURL](https://github.com/curl/curl)
 - [wxWidgets](https://github.com/wxWidgets/wxWidgets)
 (Only for Antares Simulator)

This section describes install procedures for the third-party Open source libraries used by ANTARES :
- Using VCPKG (Only tested on windows)
- Using a package manager (Only available on linux)
- Automatic librairies compilation from git


### [Using VCPKG](#vcpkg)

For Windows we will use [vcpkg](https://github.com/microsoft/vcpkg) to download and compile the librairies.

You must install the corresponding [vcpkg-triplet](https://vcpkg.readthedocs.io/en/latest/users/integration/#triplet-selection) depending on Antares version and libraries load:

- x64-windows        : 64 bits version with dynamic librairies load
- x86-windows        : 32 bits version with dynamic librairies load
- x64-windows-static : 64 bits version with static librairies load
- x86-windows-static : 32 bits version with static librairies load

The vcpkg-triplet used will be named [vcpg-triplet] later in this document.

#### 1 Install vcpkg 

vcpkg can be installed anywhere on your computer :

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Note : all vcpkg command further described must be launch from vcpkg folder. This folder will be named [vcpkg_root] later in this document.

#### 2 Install dependencies
```
cd [vcpkg_root]
vcpkg install openssl:[vcpg-triplet] 
vcpkg install curl:[vcpg-triplet] 
vcpkg install wxwidgets:[vcpg-triplet] 
```
### [Using a package manager](#linux_manager)
On linux you can use a package manger to download the precompiled librairies.

#### Ubuntu

```
sudo apt-get install libuuid1 uuid-dev
sudo apt-get install libcurl4-openssl-dev
sudo apt-get install libssl-dev
sudo apt-get install libwxgtk3.0-dev
```

#### RHEL / Centos

```
sudo yum install openssl
sudo yum install curl
sudo yum install wxGTK3-devel
```
### [Automatic librairies compilation from git](#git_compil)
Dependency can be built  at configure time using the option `-DBUILD_DEPS=ON` (`OFF` by default) or you can compile few of them using the options below.

* OPENSSL (`BUILD_OPENSSL`)
* CURL (`BUILD_CURL`)
* wxWidgets (`BUILD_wxWidgets`)

Librairies are compiled with static option. When `BUILD_CURL` option is used, `BUILD_OPENSSL` option is added.

## [Building Antares Solution](#build)
Antares source directory is named [antares_src] in all commands.

Build can be done 'out of source'.

You can define build type with ```-DCMAKE_BUILD_TYPE``` option.

release

```
cmake -DCMAKE_BUILD_TYPE=release ..
```
 debug
 ```
cmake -DCMAKE_BUILD_TYPE=debug ..
```
Note that these are not the standard CMAKE_BUILD_TYPE. CMake files must be updated.

### Windows building external librairies (recommanded)

- create a work directory (recommanded)


```bash
mkdir  Antares_workdir
cd Antares_workdir
```

Then use the following commands to configure, build and install Sirius, Ortools and finaly Antares (solver only for now).  
If you want to use a specific version of the compiler, use the -G option of cmake: ```-G"Visual Studio 15 2017 Win64``` for instance

```bash
git clone https://github.com/rte-france/sirius-solver.git -b Antares_VCPKG Sirius
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="installSirius" -B Sirius/build -S Sirius/src
cmake --build Sirius/build/ --config Release --target install -j4

git clone https://github.com/rte-france/or-tools.git -b rte_dev_sirius ortools
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="installOrtools" -B ortools/build -S ortools/ -DUSE_SIRIUS=ON -DUSE_COINOR=ON -DBUILD_PYTHON=OFF -DBUILD_TESTING=OFF -DBUILD_DEPS=ON -Dsirius_solver_ROOT=installSirius
cmake --build ortools/build/ --config Release --target install -j4

git clone https://github.com/rte-france/Antares_Simulator.git -b feature/Sirius_extraction Antares_Simulator
cmake -B Antares_Simulator/build -DCMAKE_BUILD_TYPE=release -S Antares_Simulator/src -Dsirius_solver_ROOT=installSirius -DBUILD_DEPS=ON -Dortools_ROOT=installOrtools
cmake --build Antares_Simulator/build --config Release --target antares-7.1-solver -j4
```

Note : ```-j``` option should be adapted to your computer's number of cpu/cores.

### Linux using system libs (recommanded)
- Install dependencies [using package manager](#using-a-package-manager).
- Create a work directory (recommanded)
```bash
mkdir  Antares_workdir
cd Antares_workdir
```

Then use the following commands to configure, build and install Sirius, Ortools and finaly Antares (solver only for now).  
For windows, if you want to use a specific version of the compiler, use the -G option of cmake: ```-G"Visual Studio 15 2017 Win64``` for instance

```bash
git clone https://github.com/rte-france/sirius-solver.git -b Antares_VCPKG Sirius
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="installSirius" -B Sirius/build -S Sirius/src
cmake --build Sirius/build/ --config Release --target install

git clone https://github.com/rte-france/or-tools.git -b rte_dev_sirius ortools
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="installOrtools" -B ortools/build -S ortools/ -DUSE_SIRIUS=ON -DUSE_COINOR=ON -DBUILD_PYTHON=OFF -DBUILD_TESTING=OFF -DBUILD_DEPS=ON -Dsirius_solver_ROOT=installSirius
cmake --build ortools/build/ --config Release --target install

git clone https://github.com/rte-france/Antares_Simulator.git -b feature/Sirius_extraction Antares_Simulator
cmake -B Antares_Simulator/build -DCMAKE_BUILD_TYPE=release -S Antares_Simulator/src -Dsirius_solver_ROOT=installSirius -Dortools_ROOT=installOrtools
cmake --build Antares_Simulator/build --config Release --target antares-7.1-solver
```

Note : compilation can be done on several processor with ```-j``` option.

### Window using vcpkg (not recommanded)

__Using or even installing VCPK with global intergration will break the way find_package works and is not compatible with this or-tools version__

- Install dependencies [using VCPKG](#using-vcpkg).
- Choose [vcpkg-triplet]
- Create build dir (optionnal but recommanded)
```
cd [antares_src]
mkdir _build
```
- Configure build with cmake

```
cd [antares_src]
cd _build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] ..
```
- Build
 ```
cd [antares_src]
cd _build
make
```