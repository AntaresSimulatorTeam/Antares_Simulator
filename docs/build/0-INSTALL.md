# Antares Simulator CMake Build Instructions

[Supported OS](#supported-os) | [CMake version](#cmake-version) | [Python version](#python-version) |  [Git version](#git-version) | [Dependencies](#dependencies)

## C/I status
| OS     | Status |
|:-------|--------|
| Ubuntu  | [![Status][ubuntu_precompiled_svg]][ubuntu_precompiled_link] |
| Windows  | [![Status][windows_precompiled_svg]][windows_precompiled_link] |
| Centos7  | [![Status][centos_precompiled_svg]][centos_precompiled_link] |

## [Supported OS](#supported-os)
*ANTARES* compilation is tested on :
- Windows see [INSTALL-windows.md](INSTALL-windows.md)
- Ubuntu see [INSTALL-ubuntu.md](INSTALL-ubuntu.md)
- Centos7 see [INSTALL-centos.md](INSTALL-centos.md)

## [CMake version](#cmake-version)
CMake 3.x must be used.
On some OS it is not available by default on the system.

## [Python version](#python-version)
Python 3.x is used for end-to-end test.

## [Git version](#git-version)
Git version must be above 2.15 for external dependencies build because `--ignore-whitespace` is not used by default and this option is needed during the OR-Tools compilation of ZLib to apply a patch on Windows (see https://github.com/google/or-tools/issues/1193).

## [Dependencies](#deps)
*ANTARES* depends on severals mandatory libraries.
- [Sirius Solver](https://github.com/AntaresSimulatorTeam/sirius-solver/tree/Antares_VCPKG) (fork from [RTE](https://github.com/rte-france/sirius-solver/tree/Antares_VCPKG))
- [OR-Tools](https://github.com/AntaresSimulatorTeam/or-tools/tree/rte_dev_sirius) (fork from [RTE](https://github.com/rte-france/or-tools/tree/rte_dev_sirius) based on official OR-Tools github)
- [wxWidgets](https://github.com/wxWidgets/wxWidgets)
  (Only for the complete Antares Simulator solution with GUI)
- Boost librairies : test process filesystem regex dll (Only for unit tests)

Installation of these dependencies is described in OS specific *ANTARES* install procedure.

[ubuntu_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(pre-compiled)/badge.svg
[ubuntu_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(pre-compiled)"

[windows_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG%20and%20pre-compiled)/badge.svg
[windows_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG%20and%20pre-compiled)"

[centos_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(pre-compiled)/badge.svg
[centos_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(pre-compiled)"
