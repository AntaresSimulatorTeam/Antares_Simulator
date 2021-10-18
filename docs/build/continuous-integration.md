# Continuous Integration
*ANTARES* compilation is fully tested on GitHub CI.

Here is a description of workflow with their associated status :

| OS     |  .yml | Description | Status |
|:-------|--------|--------|------|
| Ubuntu  |`ubuntu-system.yml`|Compilation with system libraries (apt-get) and pre-compiled not system libraries use| [![Status][ubuntu_precompiled_svg]][ubuntu_precompiled_link] |
| Ubuntu  |`ubuntu-system-deps-build.yml`|Compilation with system libraries (apt-get) and not system libraries compilation| [![Status][ubuntu_deps_build_svg]][ubuntu_deps_build_link] |
| Ubuntu  |`ubuntu-release.yml`|Compilation with system libraries (apt-get) and not system libraries compilation for release creation| [![Status][ubuntu_release_svg]][ubuntu_release_link]
| Windows  |`windows-vcpkg.yml`|Compilation with VCPKG use and pre-compiled not system libraries use| [![Status][windows_precompiled_svg]][windows_precompiled_link] |
| Windows  |`windows-pre-compiled-only.yml`|Compilation without VCPKG and with pre-compiled libraries use| [![Status][windows_precompiled_only_svg]][windows_precompiled_only_link] |
| Windows  |`windows-vcpkg.yml`|Compilation with VCPKG use and pre-compiled not system libraries use for release creation| [![Status][windows_release_svg]][windows_release_link] |
| Centos7  | `centos7-system.yml`|Compilation with system librairies (`yum`) and pre-compiled not system libraries use| [![Status][centos_precompiled_svg]][centos_precompiled_link] |
| Centos7  |`centos7-system-deps-build.yml`| Compilation with system librairies (`yum`) and not system libraries compilation| [![Status][centos_deps_build_svg]][centos_deps_build_link] |
| Centos7  | `centos7-system.yml`|Compilation with system librairies (`yum`) and pre-compiled not system libraries use for release creation| [![Status][centos_release_svg]][centos_release_link] |

## Additionnals workflow

 |  .yml | Description | Status |
|:-------|--------|------|
| `sonarcloud.yml`|Compilation with specifics options for sonarcloud analysis.| [![Status][sonarcloud_svg]][sonarcloud_link] |
| `build-userguide.yml`|User guide generation| [![Status][userguide_svg]][userguide_link] |
| `branchNameValidation.yml`|Check branch name for CI workflow run||


## Created Github actions


 | Name | Description |
|:-------|--------|
| `generate-user-guide-pdf`|Generate User Guide pdf using Sphinx (not supported on windows)|
| `download-extract-precompiled-libraries-zip`|Download and extract .zip precompiled libraries from antares-deps repository|
| `download-extract-precompiled-libraries-tgz`|Download and extract .tgz precompiled libraries from antares-deps repository|


[ubuntu_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(pre-compiled)/badge.svg
[ubuntu_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(pre-compiled)"

[ubuntu_release_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(Release)/badge.svg
[ubuntu_release_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(Release)"

[ubuntu_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(deps.%20compilation)/badge.svg
[ubuntu_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(deps.%20compilation)"

[windows_precompiled_only_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(pre-compiled%20only)/badge.svg
[windows_precompiled_only_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(pre-compiled%20only)"

[windows_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG and pre-compiled)/badge.svg
[windows_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG and pre-compiled)"

[windows_release_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(Release)/badge.svg
[windows_release_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(Release)"

[centos_deps_build_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(deps.%20compilation)/badge.svg
[centos_deps_build_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(deps.%20compilation)"

[centos_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(pre-compiled)/badge.svg
[centos_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(pre-compiled)"

[centos_release_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(Release)/badge.svg
[centos_release_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(Release)"

[sonarcloud_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/SonarCloud/badge.svg
[sonarcloud_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"SonarCloud"

[userguide_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Build%20Userguide%20pdf/badge.svg
[userguide_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Build%20Userguide%20pdf"
