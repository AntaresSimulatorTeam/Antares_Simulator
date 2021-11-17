# Dependencies install

*ANTARES* depends on severals mandatory libraries.

- [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
- [OR-Tools](https://github.com/AntaresSimulatorTeam/or-tools/tree/rte_dev_sirius) (fork from [RTE](https://github.com/rte-france/or-tools/tree/rte_dev_sirius) based on official OR-Tools github)
- [wxWidgets](https://github.com/wxWidgets/wxWidgets)
  (Only for the complete Antares Simulator solution with GUI)
- Boost librairies : test (Only for unit tests)

This section describes the install procedures for the third-party Open source libraries used by *ANTARES*.
The install procedure can be done

- by compiling the sources after cloning the official git repository
- by using a package manager

## Install with package manager

=== "Windows (VCPKG)"

    For Windows we will use [vcpkg](https://github.com/microsoft/vcpkg) to download and compile the libraries. vcpkg is available as a submodule in *ANTARES*.
    
    You must install the corresponding [vcpkg-triplet](https://vcpkg.readthedocs.io/en/latest/users/integration/#triplet-selection) depending on Antares version and libraries load:
    
    - ``x64-windows``        : 64 bits version with dynamic libraries load
    - ``x86-windows``        : 32 bits version with dynamic libraries load
    - ``x64-windows-static`` : 64 bits version with static libraries load
    - ``x86-windows-static`` : 32 bits version with static libraries load
    
    The vcpkg-triplet used will be named [vcpg-triplet] later in this document.
    
    - Init submodule and install vcpkg 
    
    ```
    git submodule update --init vcpkg
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```
    
    Note :
    > all vcpkg command further described must be run from vcpkg folder. This folder will be named [vcpkg_root] later in this document.
    
    
    - Install dependencies
    ```
    cd vcpkg
    vcpkg install wxwidgets boost-test --triplet [vcpg-triplet] 
    ```
=== "Centos (yum)"

    ```
    sudo yum install git redhat-lsb-core gcc gcc-c++ make wget centos-release-scl scl-utils rpm-build
    sudo yum install cmake3 devtoolset-9
    sudo yum install libuuid-devel unzip wxGTK3-devel boost-test boost-devel
    ```
=== "Ubuntu (apt-get)"

    ```
    sudo apt-get install uuid-dev libwxgtk3.0-gtk3-dev
    sudo apt-get install libboost-test-dev
    ```

## Automatic libraries compilation from git
[Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps) is used as a git submodule for automatic libraries compilation from git.

ALL dependency can be built at configure time using the option `-DBUILD_ALL=ON` (`OFF` by default). For a list of available option see [Antares dependencies compilation repository](https://github.com/AntaresSimulatorTeam/antares-deps).

Some dependencies can't be installed with a package manager. They can be built at configure step with a cmake option  : `-DBUILD_not_system=ON` (`ON` by default):

### Defining dependency install directory
When using multiple directories for antares development with multiple branches it can be useful to have a common dependency install directory.

Dependency install directory can be specified with `DEPS_INSTALL_DIR`. By default install directory is `<antares_checkout_dir>/../rte-antares-deps-<build_type>`

Note :
> `DEPS_INSTALL_DIR` is added to `CMAKE_PREFIX_PATH`

### Pre-compiled libraries download : release version only
You can download pre-compiled antares-deps archive from [Antares dependencies compilation repository][antares-deps-url]. Only release versions are available.

Note:
> For windows, you must you use a MSVC version compatible with MSVC version used in GitHub Action.

[antares-deps-url]: https://github.com/AntaresSimulatorTeam/antares-deps/releases/tag/v2.0.0-rc2

