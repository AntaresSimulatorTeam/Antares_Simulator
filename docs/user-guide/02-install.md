# Installation

## System requirements

### Operating system
*Antares* code is cross-platform. Our releases target a few specific operating systems:

- Windows 10/11
- CentOS 7.9
- Ubuntu 20.04
- OracleServer 8.9
- OracleLinux 8  

If you use any other Windows/Linux/Unix OS, you can still use *Antares*, although you will have to build it from sources 
(see next section).

### Hard drive disk

Installed alone, the Antares simulator does not require a lot of HDD space (less than 1 GB). Installation packages including companion tools (study manager, graph editor) are however significantly heavier. The proper storage of data (i.e. both Input and Output folders of Antares studies) may require a large amount of space. The disk footprint of any individual study mainly depends on:

- The size of the power system model (number of Areas, Links, Thermal clusters, etc.)
- The number of ready-made Time-Series and the number of Time-Series to be generated at runtime and stored afterward
  (see [these parameters](ts-generator/04-parameters.md#time-series-parameters)).
- The activation of output filters
  (see [thematic-trimming](solver/static-modeler/04-parameters.md#thematic-trimming) and [geographic-trimming](solver/static-modeler/04-parameters.md#geographic-trimming) parameters).
- The number of Monte-Carlo years involved in the simulation session, if the storage of detailed
  [year-by-year results](solver/static-modeler/04-parameters.md#year-by-year) is activated
- Whether [MPS export](solver/static-modeler/04-parameters.md#include-exportmps) is activated

If you encounter space issues, consider tweaking the aforementioned parameters or reducing your study size.

### Memory

The amount of RAM required for a simulation depends on:

- The size of the power system model (number of Areas, Links, Thermal clusters, etc.)
- The number of ready-made Time-Series and that of Time-Series to be generated at runtime
- The simulation [mode](solver/static-modeler/04-parameters.md#mode)
- The [unit commitment resolution mode](solver/static-modeler/04-parameters.md#unit-commitment-mode)
- If the [multi-threading](solver/optional-features/multi-threading.md) option is used

If you encounter memory issues, consider tweaking the aforementioned parameters or reducing your study size. 

## Instructions

- **Windows 10/11**: download & run installation packages, or executables, available at
  [*https://antares-simulator.org*](https://antares-simulator.org/)
  or on [GitHub](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases).
- **CentOS 7.9**: download & run installation packages, or executables, available on [GitHub](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases).
- **Ubuntu 20.04**: download & run executables, available on [GitHub](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases).
- **OracleServer 8.9**: download & run installation packages, or executables, available on [GitHub](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases).
- **OracleLinux 8**: download & run executables, available on [GitHub](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases).
- Any other Windows/Linux/Unix OS: refer to our website to see how to [build *Antares* from sources](../developer-guide/3-Build.md)
