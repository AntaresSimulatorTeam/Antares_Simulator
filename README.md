# Antares Simulator

[![Status][ubuntu_ci_svg]][ubuntu_ci_link]
[![Status][windows_ci_svg]][windows_ci_link]
[![Status][centos_ci_svg]][centos_ci_link]
[![Status][oraclelinux_ci_svg]][oraclelinux_ci_link]
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=AntaresSimulatorTeam_Antares_Simulator&metric=alert_status)](https://sonarcloud.io/dashboard?id=AntaresSimulatorTeam_Antares_Simulator)
[![License: MPL v2](https://img.shields.io/badge/License-MPLv2-blue.svg)](https://www.mozilla.org/en-US/MPL/2.0/)
[![ReadTheDocs](https://readthedocs.org/projects/antares-simulator/badge/?version=stable)](https://antares-simulator.readthedocs.io/en/stable/?badge=stable)

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

Antares Simulator is an open-source power system simulator.  
It is meant to be used by anybody placing value in quantifying the adequacy, or the
economic performance, of interconnected power systems, at short or
remote time horizons:

Transmission system operators, power producers, regulators, academics,
consultants, NGOs and all others actors concerned by energy policy issues
are welcome to use the software.

The Antares Simulator project was initiated by RTE (French Electricity
Transmission system Operator) in 2007. It has been developed since the beginning
as a cross-platform application (Windows, GNU/Linux, Unix).

Until 2018, it had been distributed under the terms of a proprietary license.

In May 2018, RTE decided to release the project under the GPLv3 license.

In January 2024, RTE, as the exclusive copyright owner, decided to switch from the GPLv3 to the MPLv2 license,
starting with the 9.0 version of Antares Simulator.

The GUI is deprecated in favor of [Antares Web](https://antares-web.readthedocs.io).

# Links:

- Antares website:  https://antares-simulator.org
- RTE web site : http://www.rte-france.com/
- Doxygen code documentation: https://antaressimulatorteam.github.io/Antares_Simulator/doxygen
- Antares Web: https://antares-web.readthedocs.io

# Installation

This software suite has been tested under:

* Ubuntu 20.04 [![Status][ubuntu_ci_svg]][ubuntu_ci_link]
* Microsoft Windows with Visual Studio 2022 (64-bit) [![Status][windows_ci_svg]][windows_ci_link]
* Centos7 [![Status][centos_ci_svg]][centos_ci_link]
* Oracle Linux [![Status][oraclelinux_ci_svg]][oraclelinux_ci_link]

Antares Simulator is built using CMake.
For installation instructions, please visit the [documentation website](https://antares-simulator.readthedocs.io/)
or [its sources](docs/developer-guide/0-Introduction.md).

# Source Code Content

* [AUTHORS](AUTHORS.txt)           - Antares Simulator authors
* [CERTIFICATE](CERTIFICATE.txt)     - A standard DCO that has to be signed by every contributor
* [CONTRIBUTING](CONTRIBUTING.md)         - How to submit patches and discuss code evolutions
* [COPYING](COPYING.txt)            - The MPL v2 license.
* [NEWS](NEWS.md)                 - Important modifications between the releases.
* [README](README.md)             - This file.
* [ROADMAP](ROADMAP.txt)            - Main orientations for further developments
* [THANKS](THANKS.txt)             - Attribution notices for external libraries and contributors.
* [resources/](resources)     - Free sample data sets.
* [src/analyzer/](src/analyzer)      - source code for the statistical analysis of historical time-series.
* [src/cmake/](src/cmake)        - files for initializing a solution ready for compilation.
* [src/distrib/](src/distrib)       - system redistributable libraries Win(x64,x86),unix.
* [src/ext/](src/ext)         - third party libraries used by Antares_Simulator: libYuni, Sirius_Solver.
* [src/libs/](src/libs)           - miscellaneous Antares_Simulator libraries.
* [src/solver/](src/solver)     - simulation and optimization part.
* [src/tools/](src/tools)        - miscellaneous tools for dataset management.
* [src/ui/](src/ui)           - Graphic user interface.

[ubuntu_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(push%20and/or%20release)/badge.svg

[ubuntu_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions/workflows/ubuntu.yml

[windows_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG%20and%20pre-compiled)/badge.svg

[windows_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions/workflows/windows-vcpkg.yml

[centos_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(push%20and/or%20release)/badge.svg

[centos_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions/workflows/centos7.yml

[oraclelinux_ci_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Oracle%208%20CI%20(push%20and/or%20release)/badge.svg

[oraclelinux_ci_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions/workflows/oracle8.yml
