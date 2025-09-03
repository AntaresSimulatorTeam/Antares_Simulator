# Docker and CMake Presets in Antares Simulator

This page explains how to use Docker to build the project and how to use CMake Presets to simplify build configuration.
It covers command line usage and integration with the CLion IDE.

## 1. Introduction

This guide helps you set up a reproducible and efficient development environment for Antares Simulator using Docker and
CMake Presets.

---

## 2. Docker: Quick Start

### 2.1. Build the Docker image (example: clang)

From the project root:

```sh
cd docker/clang
docker build -t antares/clang:latest .
```

### 2.2. Start a container and follow the developer guide as usual

```sh
docker run -it antares/clang:latest bash
```

**Drawbacks:**

- You need to download the source and dependencies inside the container.
- You lose your work when exiting the container (unless you commit the container or copy files out).

### 2.3. Mount a volume to use local source code and dependencies

To work with your local (host) source code and dependencies, mount them as volumes. This way, changes made in your IDE
are immediately visible in the container.

**Example:**

```sh
sudo docker run -it \
  -v "$PWD:/work/Antares_Simulator" \
  -v /path/to/ortools:/work/ortools \
  antares/clang:latest bash
```

- Replace `/path/to/ortools` with the path to your local ortools directory.
- Now, `/work/Antares_Simulator` and `/work/ortools` in the container point to your host files.
- You can configure and build as always just point CMAKE_PREFIX_PATH to `/work/ortools`.

**Local** = on your host (your computer).  
**Remote** = inside the Docker container.

### 2.4. Drawbacks and advantages

- **Advantage:** You keep your work and dependencies persistent, and can use your favorite editor on the host.
- **Drawback:** File permissions and line endings may differ between host and container.

---

## 3. Using Docker Service with CLion

- See the [official JetBrains documentation](https://www.jetbrains.com/help/clion/docker.html).
- Start the Docker service.
- In CLion, configure a Docker toolchain and set up a container with the correct mount points.
- Start the container from CLion or manually, and work in the container terminal while editing files in the IDE.
- See the illustration below:

![CLion Docker Service](DockerAndCMakePresetsImages/docker_service.png)

---

## 4. CMake Presets

CMake Presets are a convenient way to save and share configuration and build parameters for CMake. They are especially
useful in command line workflows to avoid repeating options, and to centralize default parameters across developers. For
more information, see the [official documentation](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html).

- The default presets are available in `CMakePresets.json` (versioned, shared).
- Custom presets should be placed in `CMakeUserPresets.json` (not versioned, user-specific). Example configurations are
  provided in `CMakeUserPresetsExample.json`.

Presets allow you to quickly configure and build the project with consistent options. In CLion, presets substitute for
manually filling the CMake options and build options in the CMake Settings window.

Presets are useful in CLI to save the parameters of the configure and build commands, and to centralize default
parameters across developers. They also allow you to easily switch between different build configurations or
environments.

### 4.1. Simple command line usage

The project provides `CMakePresets.json` and `CMakeUserPresets.json` in the `src/` folder. To configure the project with
a preset:

```sh
cmake --preset=default
```

To list available presets:

```sh
cmake --list-presets
```

### 4.2. Using CMake Presets in CLion

- Open the project in CLion.
- Go to _File > Settings > Build, Execution, Deployment > CMake_.
- Select the desired preset.
- See the [CLion CMake Presets documentation](https://www.jetbrains.com/help/clion/cmake-presets.html) for more details.

### 4.3. Presets and docker

When using Docker, ensure that paths in the presets match the container's filesystem. For example, if your source code
is mounted at `/work/Antares_Simulator`, update the `sourceDir` in your preset accordingly.

---

## 5. VCPKG

For information on installing and using vcpkg with Antares Simulator, please refer to:

- [Dependencies install](2-Dependencies-install.md)
- [Build instructions](3-Build.md)

---

## 6. Build Output Caching

To speed up rebuilds and avoid unnecessary recompilation, you can use two complementary strategies:

### 6.1. Persisting Build Output with Docker Volumes

By mounting a host directory as a volume in your Docker container, you can persist build artifacts (such as the CMake
build directory, ccache, and vcpkg cache) across different container lifetimes. This avoids losing build output when the
container is removed.

**Example:**

```sh
docker run -it \
  -v "$PWD:/work/Antares_Simulator" \
  -v "$PWD/_build":/tmp/build \
  antares/clang:latest bash
```

```sh
cmake --preset=default --build-dir=/tmp/build
```

### 6.2. Using ccache

`ccache` is a compiler cache that stores previously compiled object files to speed up subsequent builds.

- Set the following varible at configure time or in a preset to use `ccache`:
    - Variable: `CMAKE_CXX_COMPILER_LAUNCHER=ccache`
    - Cache directory: `CCACHE_DIR=/tmp/deps/ccache`
- Make sure the cache directory is persistent (see above) to benefit from caching across container runs.

```sh
docker run -it \
  -v "$PWD:/work/Antares_Simulator" \
  -v "$PWD/_build":/tmp/build \
  -v "$PWD/ccache":/tmp/deps/ccache \
  antares/clang:latest bash
```

```sh
export CCACHE_DIR=/tmp/deps/ccache
cmake --preset=with_ccache --build-dir=/tmp/build
```

### 6.3. vcpkg caching

- vcpkg binaries are stored in `/tmp/deps/vcpkg_cache/binary-cache`.
- Installation options are configured via `VCPKG_INSTALL_OPTIONS` in the preset.
- The environment variables `VCPKG_BINARY_SOURCES` and `VCPKG_INSTALL_OPTIONS` allow reuse of binaries and avoid
  unnecessary recompilations.

---

## 7. Tips and Best Practices

- Modify or add your own presets in `CMakeUserPresets.json` (not versioned).
- For reproducible builds, use the shared presets in `CMakePresets.json`.
- Ensure cache paths are persistent if you use Docker.

For any questions or issues, consult the official documentation or contact the project maintainers.
