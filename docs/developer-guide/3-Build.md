# Build

Before building, make sure that dependencies are [installed](2-Dependencies-install.md).

## Install VCPKG

Although you may install third party dependencies yourself, the preferred way is
to rely on [vcpkg](https://github.com/microsoft/vcpkg) and its CMake integration
to build and install most of them.

The first step will be to install VCPKG using its bootstrap script:

=== "Windows"

    ```
    git submodule update --init vcpkg
    cd vcpkg
    .\bootstrap-vcpkg.bat
    ```

=== "Linux"

    ```
    git submodule update --init vcpkg
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ```

## Configure build with CMake

The preferred way of building the project is to use a pre-compiled version of OR-Tools and to install
other dependencies using vcpkg. To achieve this, you will need to define VCPKG-related variables,
and add your OR-tools install path to `CMAKE_PREFIX_PATH`:


=== "Windows"

    ```
    cmake -B _build -S src -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
                           -DVCPKG_TARGET_TRIPLET=x64-windows-antares \
                           -DCMAKE_BUILD_TYPE=Release \
                           -DCMAKE_PREFIX_PATH=</path/to/or-tools>
    ```

    > **Note:** cpack NSIS installer creation needs an 'out-of-source build'. The build directory must be outside `src` directory


=== "Linux"

    ```
    cmake -B _build -S src -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
                           -DVCPKG_TARGET_TRIPLET=x64-linux-antares \
                           -DCMAKE_BUILD_TYPE=Release \
                           -DCMAKE_PREFIX_PATH=</path/to/or-tools>
    ```

Here is a list of mandatory or optional CMake configuration options:

| Option                 | Mandatory    | Description                                                                      | Expected value                              | Default value                                             |
|:-----------------------|--------------|----------------------------------------------------------------------------------|---------------------------------------------|-----------------------------------------------------------|
| `CMAKE_C_COMPILER`     | OS-dependent | Select C compiler                                                                | `gcc-10`                                   |                                                           |
| `CMAKE_CXX_COMPILER`   | OS-dependent | Select C++ compiler                                                              | `g++-10`                                    |                                                           |
| `CMAKE_BUILD_TYPE`     | **yes**      | Define build type                                                                | `Release` / `Debug` / `RelWithDebInfo`      |                                                           |
| `BUILD_UI`             | no           | Enable or disable Antares Simulator UI[^1] compilation                           | `ON` / `OFF`                                | `ON`                                                      |
| `BUILD_TESTING`        | no           | Enable build for unit tests                                                      | `ON` / `OFF`                                | `OFF`                                                     |
| `BUILD_ORTOOLS`        | no           | Enable build for OR-Tools and its dependencies (requires an Internet connection) | `ON` / `OFF`                                | `OFF`                                                     |
| `CMAKE_TOOLCHAIN_FILE` | no           | Path to VCPKG toolchain file, allows to integrate VCPKG with cmake build         | `../vcpkg/scripts/buildsystems/vcpkg.cmake` |                                                           |
| `VCPKG_TARGET_TRIPLET` | no           | Define VCPKG triplet (build type for dependencies etc.)                          | `x64-windows-antares` / `x64-linux-antares` |                                                           |

> 💡 **Disable the UI build to make builds faster**
> The UI takes up a good chunk of compilation time. It is enabled by default, but you can disable it by turning off `BUILD_UI`

> 💡 **Use Ninja to speed up target generation by CMake**
> At configure time, you may specify Ninja for generation instead of traditional Make. This will speed up the update
> step after you make small changes to the code.
> ```
> cmake -S src [...] -G Ninja
> ```
> Note that you may need to install Ninja first (package `ninja-build` on Ubuntu).

## Build
=== "Windows"

    ```
    cmake --build _build --config Release -j8
    ```
=== "CentOS"

    ```
    cmake3 --build _build -j8
    ```
=== "Ubuntu/Debian"

    ```
    cmake --build _build -j8
    ```

> 💡 Compilation can be done on several processors with `-j` option.


## Developer tips

### Use a compiler cache
In order to avoid unnecessary rebuilds, for example when you switch branches, you may use a compiler cache
such as ccache. Using it under Linux systems is pretty easy with CMake, you only need to specify it
as the compiler launcher at configure time:
```
cmake ... -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ...
```

### Ignore submodules to make git operations faster
Antares_Simulator is quite a large project, with a few large submodules. In file .git/config, you can add this line to all [submodule] sections
```
ignore = all
```
This way git won't waste time computing diff on these when checking out, diffing commits, etc. git operations should be a lot faster.
Keep in mind that your submodules won't be updated.


[^1]: GUI support has been dropped in favor of [Antares Web](https://antares-web.readthedocs.io)
