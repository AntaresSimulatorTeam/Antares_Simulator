---
hide:
 - toc
---

# Study Cleaner


> _**WARNING:**_ this feature is deprecated and will be removed in a future release. If you are still using it,
> please [get in touch](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues) with us.

**Executable**: antares-study-cleaner (currently released for Windows & Ubuntu only)

| command           | meaning                                   |
|:------------------|:------------------------------------------|
| -i, --input=VALUE | An input folder where to look for studies |
| --dry             | List the folder only and do nothing       |
| --mrproper        | Suppress the outputs and logs files       |
| -v, --version     | Print the version and exit                |
| -h, --help        | Display this help and exit                |


#### Note about the "user" subdirectory
*Antares* is not allowed to delete any files from the "user" subdirectory of the study directory. 
As a consequence, the "user" subdirectory is unaffected by the Study Cleaner.