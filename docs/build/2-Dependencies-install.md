---
toc_depth: 2
---

# Dependencies install

*Antares Simulator* depends on some mandatory libraries.

- [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
- [OR-Tools](https://github.com/rte-france.com/or-tools) (fork from [Google](https://github.com/google/or-tools))
- [wxWidgets](https://github.com/wxWidgets/wxWidgets)
  (Only for the complete Antares Simulator solution with GUI)
- [Boost](https://www.boost.org/) libraries: test (Only for unit tests)

These third-party libraries can be installed:
- by building them from sources (see [here](3-Build.md) for more information)
- or using a package manager.

## Install with package manager

=== "Windows"

    For Windows we will use [vcpkg](https://github.com/microsoft/vcpkg) to download and compile the libraries. vcpkg is available as a submodule in *ANTARES*.
    
    You must install the corresponding [vcpkg-triplet](https://vcpkg.readthedocs.io/en/latest/users/integration/#triplet-selection) depending on Antares version and libraries load:
    
    - ``x64-windows``       : 64 bits version with dynamic libraries load
    - ``x86-windows``       : 32 bits version with dynamic libraries load
    - ``x64-windows-static``: 64 bits version with static libraries load
    - ``x86-windows-static``: 32 bits version with static libraries load
    
    The vcpkg-triplet used will be named [vcpg-triplet] later in this document.
    
    - Init submodule and install vcpkg 
    
    ```
    git submodule update --init vcpkg
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```
    
    Note:
    > all vcpkg command further described must be run from vcpkg folder. This folder will be named [vcpkg_root] later in this document.
    
    
    - Install dependencies
    ```
    cd vcpkg
    vcpkg install wxwidgets boost-test --triplet [vcpg-triplet] 
    ```
=== "Centos"

    ```
    sudo yum install git redhat-lsb-core gcc gcc-c++ make wget centos-release-scl scl-utils rpm-build
    sudo yum install cmake3 devtoolset-9
    sudo yum install libuuid-devel unzip wxGTK3-devel boost-test boost-devel
    ```
=== "Ubuntu 20.04 or 22.04 / Debian 11"

    ```
    sudo apt install uuid-dev libwxgtk3.0-gtk3-dev
    sudo apt install libboost-test-dev
    ```

=== "Ubuntu 23.04 / Debian 12"

    ```
    sudo apt install uuid-dev libwxgtk3.2-dev libboost-test-dev
    ```

## Automatic libraries compilation from sources
[The Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is a submodule that can be used for automatic libraries compilation from sources.

Apart from OR-Tools, all dependencies can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default). 
For a list of available options, see [the Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

You can set `-DBUILD_ORTOOLS=ON` to download & build OR-Tools. It is also possible to use a precompiled archive, see below.

For compiling the package yourself from git, additional build dependencies are needed (see [here](1-Development-requirements.md)).

### Pre-compiled OR-Tools: release+static only
You can [download](https://github.com/rte-france/or-tools/releases) a precompiled OR-Tools archive that contains headers & static libraries.

Please note that dynamic linking with OR-Tools is only supported in Linux.

Decompress the archive, and provide its path as a `CMAKE_PREFIX_PATH`. If you use XPRESS, you may need also to specify `XPRESS_ROOT`.

### Defining dependency install directory
When using multiple directories for Antares development with multiple branches, it can be useful to have a common dependency install directory.

Dependency install directory can be specified with `DEPS_INSTALL_DIR`. By default, install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note:
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

### Pre-compiled libraries download: release version only
You can download a pre-compiled antares-deps archive from [Antares dependencies compilation repository][antares-deps-url]. Only release versions are available.

Note:
> For Windows, you must you use a MSVC version compatible with the MSVC version used in the GitHub Action that made the release.

[antares-deps-url]: https://github.com/AntaresSimulatorTeam/antares-deps/releases

