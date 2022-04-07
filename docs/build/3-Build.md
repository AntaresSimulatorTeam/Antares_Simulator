# Build
Before build, make sure that dependencies are installed (see [here](2-Dependencies-install.md) for more informations)

## Environnement settings
On Centos enable `devtoolset-9` and `rh-git227`:
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
    Note :
    > cpack NSIS installer creation need an 'out of source build'. The build directory must be outside `[antares_src]` directory
    
    ```
    cmake -B _build -S [antares_src] -DVCPKG_ROOT=[vcpkg_root] -DVCPKG_TARGET_TRIPLET=[vcpkg-triplet] -DCMAKE_BUILD_TYPE=release
    ```
=== "Centos"

    ```
    cmake3 -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
    ```
=== "Ubuntu"

    ```
    cmake -B _build -S [antares_src] -DCMAKE_BUILD_TYPE=release
    ```

Here is a list of available CMake configure option :

|Option | Description |
|:-------|-------|
|`CMAKE_BUILD_TYPE` | Define build type. Available values are `release` and `debug`  |
|`BUILD_UI`|Enable or disable Antares Simulator UI compilation (default `ON`)|
|`BUILD_not_system`|Enable build of external libraries not available on system package manager (default `ON`)|
|`BUILD_ALL`|Enable build of ALL external libraries (default `OFF`)|
|`DEPS_INSTALL_DIR`|Define dependencies libraries install directory|
|`USE_PRECOMPILED_EXT`| This option must be set if you use wxWidget as precompiled external library (default `OFF`)|
|`BUILD_TESTING`| Enable test build (default `OFF`)|

Additionnal options for windows

|Option |Description |
|:-------|-------|
|`VCPKG_ROOT`|Define vcpkg directory |
|`VCPKG_TARGET_TRIPLET`|Define [vcpkg-triplet] |

## Build
=== "Windows"

    ```
    cmake --build _build --config release -j8
    ```
=== "Centos"

    ```
    cmake3 --build _build --config release -j8
    ```
=== "Ubuntu"

    ```
    cmake --build _build --config release -j8
    ```
Note :
> Compilation can be done on several processor with ```-j``` option.
