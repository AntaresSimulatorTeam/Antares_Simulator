# Antares Simulator
[![Status][ubuntu_precompiled_svg]][ubuntu_precompiled_link]  [![Status][windows_precompiled_svg]][windows_precompiled_link] [![Status][centos_precompiled_svg]][centos_precompiled_link] [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=AntaresSimulatorTeam_Antares_Simulator&metric=alert_status)](https://sonarcloud.io/dashboard?id=AntaresSimulatorTeam_Antares_Simulator)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)



Antares Simulator is an open source power system simulator meant to be
used by anybody placing value in quantifying the adequacy or the 
economic performance of interconnected power systems, at short or 
remote time horizons: 

Transmission system Operators, Power Producers, Regulators, Academics,
Consultants, NGO and all others actors concerned by energy policy issues
are welcome to use the software.

The Antares Simulator project was initiated by RTE (French Electricity 
Transmission system Operator) in 2007. It was developed from the start
as a cross-platform application (Windows, GNU/Linux ,Unix). 

Until 2018 it was distributed under the terms of a proprietary license.

In May 2018 RTE decided to release the project under the GPLv3 license.

# Links:

- Antares web site :  https://antares-simulator.org
- RTE web site  : http://www.rte-france.com/


# Installation

This software suite has been tested under:

*   Ubuntu 20.04 [![Status][ubuntu_precompiled_svg]][ubuntu_precompiled_link] 
*   Microsoft Windows with Visual Studio 2019 (64-bit) [![Status][windows_precompiled_svg]][windows_precompiled_link]
*   Centos7 [![Status][centos_precompiled_svg]][centos_precompiled_link] 

Antares Simulator is built using CMake.
For installation instructions, please visit [INSTALL.md](INSTALL.md)

# Source Code Content

* [AUTHORS](AUTHORS.txt)           -    Antares Simulator authors
* [CERTIFICATE](CERTIFICATE.txt)	 - A standard DCO that has to be signed by every contributor 
* [CONTRIBUTING](CONTRIBUTING.txt)	     - How to submit patches and discuss about code evolutions
* [COPYING](COPYING.txt)            - The GPL v3 license.
* [INSTALL](INSTALL.md)           - Installation and building instructions.
* [NEWS](NEWS.md)                 - Important modifications between the releases.
* [README](README.md)             - This file.
* [ROADMAP](ROADMAP.txt)            - Main orientations for further developements 
* [THANKS](THANKS.txt)             - Attribution notices for external libraries and contributors.
* [resources/](resources)	 - Free sample data sets. 
* [src/analyzer/](src/analyzer)      - source code for the statistical analysis of historical time-series.
* [src/cmake/](src/cmake)        - files for initializing a solution ready for compilation. 
* [src/distrib/](src/distrib)       - system redistributable libraries  Win(x64,x86),unix. 
* [src/ext/](src/ext)     	 - third party libraries used by Antares_Simulator: libYuni, Sirius_Solver.
* [src/libs/](src/libs)		   - miscellaneous Antares_Simulator libraries.
* [src/internet/](src/internet)      - web access (check for updates, usage metrics, data exchange).
* [src/simulator/](src/simulator)     - Time-series generation, Monte-Carlo simulation and weekly optimization modelling.
* [src/tools/](src/tools)        - miscellaneous tools for dataset management. 
* [src/ui/](src/ui)           - Graphic user interface. 


[ubuntu_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Ubuntu%20CI%20(pre-compiled)/badge.svg
[ubuntu_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Ubuntu%20CI%20(pre-compiled)"

[windows_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Windows%20CI%20(VCPKG%20and%20pre-compiled)/badge.svg
[windows_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Windows%20CI%20(VCPKG%20and%20pre-compiled)"

[centos_precompiled_svg]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Centos7%20CI%20(pre-compiled)/badge.svg
[centos_precompiled_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Centos7%20CI%20(pre-compiled)"
