# Build
Before build, make sure that dependencies are [installed](2-Dependencies-install.md).

## Environment settings
On CentOS, enable `devtoolset-9` and `rh-git227`:
```
scl enable devtoolset-9 bash
source /opt/rh/rh-git227/enable
```
## Update git submodule
```
git submodule update --init src/antares-deps
```
## Configure build with CMake
=== "Windows"
    Note:
    > cpack NSIS installer creation need an 'out of source build'. The build directory must be outside `[antares_src]` directory

    ```
    cmake -B _build -S [antares_src] -DVCPKG_ROOT=[vcpkg_root] -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] -DCMAKE_BUILD_TYPE=release
    ```
=== "CentOS"

    ```
    cmake3 -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
    ```
=== "Ubuntu/Debian"

    ```
    cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
    ```

Here is a list of mandatory or optional CMake configuration options:

| Option                | Mandatory | Description                                                                      | Expected value                         | Default value                                             |
|:----------------------|-----------|----------------------------------------------------------------------------------|----------------------------------------|-----------------------------------------------------------|
| `CMAKE_C_COMPILER`    | **yes**   | Select C compiler                                                                | `gcc_-0`                               |                                                           |
| `CMAKE_CXX_COMPILER`  | **yes**   | Select C++ compiler                                                              | `g++-10`                               |                                                           |
| `CMAKE_BUILD_TYPE`    | **yes**   | Define build type                                                                | `Release` / `Debug` / `RelWithDebInfo` |                                                           |
| `BUILD_UI`            | no        | Enable or disable Antares Simulator UI compilation                               | `ON` / `OFF`                           | `ON`                                                      |
| `BUILD_ALL`           | no        | Enable build of ALL external libraries                                           | `ON` / `OFF`                           | `OFF`                                                     |
| `DEPS_INSTALL_DIR`    | no        | Define dependencies libraries install directory                                  | absolute path to an existing directory | `<antares_checkout_dir>/../rte-antares-deps-<build_type>` |
| `USE_PRECOMPILED_EXT` | no        | This option must be set if you use wxWidget as precompiled external library      | `ON` / `OFF`                           | `OFF`                                                     |
| `BUILD_TESTING`       | no        | Enable build for unit tests                                                      | `ON` / `OFF`                           | `OFF`                                                     |
| `BUILD_ORTOOLS`       | no        | Enable build for OR-Tools and its dependencies (requires an Internet connection) | `ON` / `OFF`                           | `OFF`                                                     |

Additional options for windows

| Option                 | Description            |
|:-----------------------|------------------------|
| `VCPKG_ROOT`           | Define vcpkg directory |
| `VCPKG_TARGET_TRIPLET` | Define [vcpkg-triplet] |

## Build
=== "Windows"

    ```
    cmake --build _build --config release -j8
    ```
=== "CentOS"

    ```
    cmake3 --build _build --config release -j8
    ```
=== "Ubuntu/Debian"

    ```
    cmake --build _build --config release -j8
    ```
Note:
> Compilation can be done on several processors with `-j` option.

The final GUI file can be executed at `_build/ui/simulator/antares-8.7-ui-simulator`