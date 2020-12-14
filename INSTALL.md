# Antares Simulator CMake Build Instructions

[Environnement](#environment) | [CMake version](#cmake-version) | [Git version](#git-version) | [Dependencies](#dependencies) | [Building](#building-antares-solution) | [Unit tests](#unit-tests) | [Installer creation](#installer-creation)

## C/I status
| OS     | System librairies | VCPKG | Built in libraries |
|:-------|-----|--------|------|
| Linux  | [![Status][linux_system_svg]][linux_system_link] |Not available  | [![Status][linux_deps_build_svg]][linux_deps_build_link] |
| Windows  | Not available|  [![Status][windows_vcpkg_svg]][windows_vcpkg_link] | [![Status][windows_deps_build_svg]][windows_deps_build_link] |


[linux_system_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(system%20libs)/badge.svg

[linux_system_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(system%20libs)"

[linux_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux%20CI%20(deps.%20compilation)/badge.svg

[linux_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux%20CI%20(deps.%20compilation)"

[windows_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(deps.%20compilation)/badge.svg

[windows_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(deps.%20compilation)"

[windows_vcpkg_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG)/badge.svg

[windows_vcpkg_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG)"

Note : Linux with VCPKG is not available due to issue in Find_wxWidgets.cmake with VCPKG use in linux.

## [Environment](#env)
ANTARES Solver/Simulator is a cross-platform project using components compatible
with many 64 bits OS (Windows, Linux, Unix). 

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

To build on Windows system, please change end-of-line style to appropriate standard. 
Note that Visual Studio may carry out auto-reformating. 
 
## [CMake version](#cmake-version)
CMake 3.x must be used.
On some OS it is not available by default on the system.

### Ubuntu
There are several ways to get CMake 3.x on your system.

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

### RHEL / Centos
```
sudo yum install cmake3
```
Note:
> All ``cmake``  commands must be replaced by ``cmake3``.

## [Git version](#git-version)
Git version must be above 2.15 for external dependencies build because `--ignore-whitespace` is not used by default and this option is needed during the OR-Tools compilation of ZLib to apply a patch on Windows (see https://github.com/google/or-tools/issues/1193).

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
- by using a package manager. Depending on the OS we propose a solution
  - using VCPKG (Only tested on windows)
  - using the official package manager of the linux distribution


### [Using VCPKG](#vcpkg)

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
### [Using a package manager](#linux_manager)
On linux you can use a package manger to download the precompiled librairies.

#### Ubuntu

##### 20.04 (Focal)
```
sudo apt install uuid-dev libcurl4-openssl-dev libssl-dev libwxgtk3.0-gtk3-dev libboost-test-dev
```

##### 16.04 (Xenial) and 18.04 (Bionic)
```
sudo apt-get install uuid-dev libcurl4-openssl-dev libssl-dev libwxgtk3.0-dev libboost-test-dev
```

#### RHEL / Centos

```
sudo yum install openssl
sudo yum install curl
sudo yum install wxGTK3-devel
sudo yum install boost-test boost-filesystem boost-regex boost-devel
```
### [Automatic librairies compilation from git](#git_compil)
[Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is used as a git submodule for automatic librairies compilation from git.

Dependency can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default). For a list of available option see [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

We recommend you to define the options below at your first antares simulator checkout :

* Sirius solver (`-DBUILD_sirius=ON`)
* OR-Tools (`-DBUILD_ortools=ON`)

This allow the compilation of the librairies that are not available in package manager.


#### Defining dependency install directory
When using multiple directories for antares development with multiple branches it can be useful to have a common dependency install directory.

Dependency install directory can be specified with `DEPS_INSTALL_DIR`. By default install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note :
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

## [Building Antares Solution](#build)

Antares source directory is named `[antares_src]` in all following commands.

First you need to update git submodule for dependency build :
```
git submodule update --init [antares_src]/antares-deps
```

Note :
> cpack NSIS installer creation need an 'out of source build'. The build directory must be outside `[antares_src]` directory

### Complete solution including GUI

You can define build type with ```-DCMAKE_BUILD_TYPE``` and ```--config``` option.

- release

```
cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
cmake --build _build --config release
```
-  debug
 ```
cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=debug
cmake --build _build --config debug
```
Note :
> These are not the standard ``CMAKE_BUILD_TYPE``. Antares CMake files might need future updates.

### Antares Solver and other command line tools (w/o GUI)

Antares Simulator UI application compilation can be disabled at configure time using the option `-DBUILD_UI=OFF` (`ON` by default)

### Sirius solver and OR-Tools linking

By default Sirius solver and OR-Tools are NOT compiled with Antares Solution.
You can enable compilation with `-DBUILD_sirius=ON -DBUILD_ortools=ON` when you configure build with cmake.

We recommend you to use `-DDEPS_INSTALL_DIR` option so you can use these builds in another antares checkout directory.

In this case you can specify dependency install directory with :
```
cmake -DDEPS_INSTALL_DIR=<deps_install_dir>
````
### Linux using system libs (recommended)
- Install dependencies [using package manager](#using-a-package-manager).

- Configure build with cmake
```
cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
```
- Build
 ```
cmake --build _build --config release -j8
```
Note :
>Compilation can be done on several processor with ```-j``` option.

### Window using vcpkg (recommended)
- Install dependencies [using VCPKG](#using-vcpkg).
- Choose [vcpkg-triplet]

- Configure build with cmake

```
cmake -B _build -S [antares_src] -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] -DCMAKE_BUILD_TYPE=release
```

or 

```
cmake -B _build -S [antares_src] -DVCPKG_ROOT=[vcpkg_root] -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] -DCMAKE_BUILD_TYPE=release
```
Note :
> Configuring with the ```-DVCPKG_ROOT``` option, removeq some CMake warnings (mainly [CMP0012](https://cmake.org/cmake/help/v3.0/policy/CMP0012.html) policy warnings)

- Build
 ```
cmake --build _build --config release -j8
```
Note :
> Compilation can be done on several processor with ```-j``` option.
### Linux/Window building external librairies

- Configure build with CMake using ```BUILD_DEPS``` option.

```
cmake -B _build -S [antares_src] -DBUILD_DEPS=ON -DCMAKE_BUILD_TYPE=release ..
```
- Build
 ```
cmake --build _build --config release -j8
```
Note :
> Compilation can be done on several processors with ```-j``` option.

## [Unit tests](#unit-tests)

Unit tests compilation  can be enabled at configure time using the option `-DBUILD_TESTING=ON` (`OFF` by default)

After build, unit tests can be run with ``ctest`` :
 ```
cd _build
ctest -C Release --output-on-failure
```

## [Installer creation](#installer)
CPack can be used to create the installer after the build pahse, depending on operating system.

### Window using NSIS
 ```
cd _build
cpack -GNSIS
```
Currently missing in NSIS installer :
- External librairies sources

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
