---
toc_depth: 2
---

# Dependencies install

*Antares Simulator* depends on some mandatory libraries.

- [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
- [OR-Tools](https://github.com/rte-france.com/or-tools) (fork from [Google](https://github.com/google/or-tools))
- [wxWidgets](https://github.com/wxWidgets/wxWidgets)
  (Only for the complete Antares Simulator solution with GUI)
- [minizip](https://github.com/zlib-ng/minizip-ng) library, with its dependency zlib
- [Boost](https://www.boost.org/) libraries: header libraries and boost-test library
- [libuuid](https://linux.die.net/man/3/libuuid) on Linux systems

We favor using [vcpkg](https://github.com/microsoft/vcpkg) for building and installing most of those dependencies,
see [build instructions](3-Build.md) which explain how it integrates with CMake build.
However, we still have a few exceptions that must be installed in a different way, see next sections.

Although not encouraged, it's still possible to install those dependencies yourself and add 
their installation path to your `CMAKE_PREFIX_PATH`.

## OR-Tools

OR-Tools may be installed in one of 2 ways:

1. **As a pre-compiled dependency** 

    You can [download](https://github.com/rte-france/or-tools/releases) a precompiled OR-Tools archive that contains headers & static libraries.
    
    Please note that dynamic linking with OR-Tools is only supported in Linux.
    
    Decompress the archive, and provide its path as a `CMAKE_PREFIX_PATH`.

2. **As part of the build**

    You may enable the `BUILD_ORTOOLS` configuration option to build it from source during Antares build,
    as documented in [build instructions](3-Build.md).

    The drawback of this second approach is that OR-Tools may need to be built again when you
    run again a cmake configure step, therefore it's not advised for developers.


## Linux: libuuid and wxWidgets

On Linux systems, libuuid development packages need to be installed with your 
OS package manager.
You will also need to install wxWidgets if you want to build the GUI.

=== "Centos"

    ```
    sudo yum install libuuid-devel
    sudo yum install wxGTK3-devel
    ```

=== "Ubuntu 20.04 or 22.04 / Debian 11"

    ```
    sudo apt install uuid-dev
    sudo apt install libwxgtk3.0-gtk3-dev
    ```

=== "Ubuntu 23.04 / Debian 12"

    ```
    sudo apt install uuid-dev
    sudo apt install libwxgtk3.2-dev
    ```
