CentOS 7 handling is implemented through a **multi-stage Docker build process** orchestrated by GitHub Actions. It is not built directly on the runner but inside isolated container images.

Here is the breakdown:

### 1. Dependency Image (`docker/AntaresDeps`)
This is the base image providing the CentOS 7 environment and build tools.
*   **Base:** `FROM centos:7`.
*   **Repo Fix:** It modifies `/etc/yum.repos.d/*.repo` to point to `vault.centos.org` because CentOS 7 has reached EOL and its standard mirrors are archived.
*   **Tools:** Installs `devtoolset-11` (GCC 11), `cmake` (via pip), `python3`, `ccache`

### 2. Build Image (`docker/Dockerfile`)
This image inherits from the dependency image and handles the actual compilation.
*   **Base:** `FROM antaresrte/antaressystemdeps:latest`.
*   **Sources:** Clones the `Antares_Simulator` repository (branch specified via `ARG BRANCH`).
*   **Dependencies:** Initializes `vcpkg` submodules and downloads external dependencies like OR-Tools.
*   **Configuration:** Uses `ccache` to speed up builds and sets up the build directory (`_build`).
*   **Compilation:** Runs `cmake` and `make` using `devtoolset-11`.
*   **Packaging:** Generates `.rpm` and `.tar.gz` archives using `cpack` and creates a solver tarball.

### 3. CI/CD Orchestration (`centos7.yml`)
The GitHub Actions workflow manages the lifecycle:
*   **Trigger:** Runs on push to `develop`, `dependabot/**`, `release/*`, or on a daily schedule (`cron`).
*   **Caching:**
    *   **vcpkg:** Caches binary dependencies (`vcpkg_cache`) to avoid re-downbuilding libraries.
    *   **ccache:** Caches compiled object files (`/.ccache`) to accelerate incremental builds.
*   **Docker Image Publishing:** Checks if `docker/AntaresDeps` changed. If so, it builds and pushes the base image to Docker Hub (`antaresrte/antaressystemdeps:latest`).
*   **Build Execution:** Builds the main `Dockerfile`, extracts the resulting artifacts (`.tgz`, `.rpm`) from the container, and uploads them as GitHub Actions artifacts.
*   **Release Management:**
    *   On `release` creation: Uploads artifacts to the GitHub Release.
    *   On `develop` branch: Overwrites the `continuous-delivery` release with the latest build.

### Key Characteristics
*   **EOL Handling:** Explicitly uses `vault.centos.org` for package installation.
*   **Modern Toolchain on Legacy OS:** Uses `devtoolset-11` to provide a modern C++ compiler on the legacy CentOS 7 OS.
*   **Isolation:** All builds happen in Docker containers to ensure reproducibility and environment consistency.
*   **Caching Strategy:** Uses both `actions/cache` and `ccache` to reduce build times for dependencies and source code.
