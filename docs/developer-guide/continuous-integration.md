# Continuous Integration
*ANTARES SIMULATOR*'s compilation is fully tested on GitHub CI.

Here is a description of workflows with their associated status.

## Build & release workflows

| OS               | .yml                  | Description                                                                             | Status                                               |
|:-----------------|-----------------------|-----------------------------------------------------------------------------------------|------------------------------------------------------|
| Ubuntu           | `ubuntu.yml`          | Builds Antares Simulator                                                                | [![Status][ubuntu_ci_svg]][ubuntu_ci_link]           |
| Windows          | `windows-vcpkg.yml`   | Builds Antares Simulator                                                                | [![Status][windows_ci_svg]][windows_ci_link]         |
| Centos7          | `centos7.yml`         | Builds Antares Simulator                                                                | [![Status][centos_ci_svg]][centos_ci_link]           |
| OracleLinux8     | `oracle8.yml`         | Builds Antares Simulator                                                                | [![Status][oraclelinux_ci_svg]][oraclelinux_ci_link] |
| -                | `build-userguide.yml` | Generates PDF user guide from material in `docs/user-guide` (uses Sphinx)               | [![Status][userguide_svg]][userguide_link]           |
| All of the above | `new_release.yml`     | When a release is created, runs the aforementioned workflows and publishes their assets | [![Status][new_release_svg]][new_release_link]       |

## Additional workflows

| .yml                                                    | Description                                                                                                       | Status                                       |
|:--------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------|----------------------------------------------|
| `sonarcloud.yml`                                        | Compilation with specific options for sonarcloud analysis                                                         | [![Status][sonarcloud_svg]][sonarcloud_link] |
| `branchNameValidation.yml`                              | This workflow verifies that new branches respect Antares Simulator Team's naming conventions[^1]                  |                                              |
| `docker.yml`                                            | Builds and pushes ["centos7-basic-requirements" docker image](https://hub.docker.com/r/antaresrte/rte-antares)    |                                              |
| `doxygen.yml`                                           | Builds & publishes the [doxygen](https://antaressimulatorteam.github.io/Antares_Simulator/doxygen/) documentation |                                              |
| `download-extract-precompiled-libraries-tgz/action.yml` | Download and extract .tgz precompiled libraries from antares-deps repository                                      |                                              |
| `download-extract-precompiled-libraries-zip/action.yml` | Download and extract .zip precompiled libraries from antares-deps repository                                      |                                              |
| `install-cmake-328/action.yml`                          | Installs cmake 3.28 using devtoolset 10                                                                           |                                              |
| `run-tests/action.yml`                                  | Runs tests on the simulator using reference study batches [here](https://github.com/AntaresSimulatorTeam/SimTest) |                                              |
| `clang-format.yml`                                      | Check formatting using clang-format 18.1.3 through bash script src/format-code.sh[^2]                                      |                                     |
[^1]: all branch names must start with `feature/`, `features/`, `fix/`, `release/`, `doc/`, `issue-`, or `dependabot/`; otherwise the workflows are not run
[^2]: please note that this job must succeed in order to merge PRs

[ubuntu_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(push%20and/or%20release)/badge.svg
[ubuntu_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(push%20and/or%20release)"

[windows_ci_only_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(pre-compiled%20only)/badge.svg
[windows_ci_only_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(pre-compiled%20only)"

[windows_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG%20and%20pre-compiled)/badge.svg
[windows_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG%20and%20pre-compiled)"

[centos_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(push%20and/or%20release)/badge.svg
[centos_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(push%20and/or%20release)"

[oraclelinux_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Oracle%208%20CI%20(push%20and/or%20release)/badge.svg
[oraclelinux_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Oracle%208%20CI%20(push%20and/or%20release)"

[sonarcloud_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/SonarCloud/badge.svg
[sonarcloud_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"SonarCloud"

[userguide_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Build%20Userguide%20pdf/badge.svg
[userguide_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Build%20Userguide%20pdf"

[new_release_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Create%20new%20release/badge.svg
[new_release_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Create%20new%20release"
