# Use a minizip-ng to write output files into a single zip archive
Create a single zip archive to keep all output files, including mc-ind, mc-all, MPS files, TS numbers, etc.

## Status: accepted

## Context
To store the simulation results, Antares Simulator produces a hierarchy of several directories, each containing multiple files. Depending on simulation parameters, for a typical study, several thousands of CSV files are produced.

In some cases, the user wants their output into a single zip file. This is the case when the simulation is run on a remote host such as an HPC. In that case, the user wants to download the results at the end of the simulation to inspect them on a different machine. Downloading individual files would take too long, so they first zip their study. The zipping can sometimes take 30+ min and uses a lot of I/O, but not much CPU.

## Decision
* Use library minizip-ng to manage the zip archive
* Use a dedicated thread to write into that archive to avoid concurrency issues
* Use a generic C++ interface to hide the above implementation details and remain compatible with files & directories (current format)
* When the zip output is chosen, install signal handlers to properly close the zip file to make sure it is finalized & valid

minizip-ng was chosen because unlike zlib, it does not make copies of the archive when adding new entries. Doing so leads to a massive drop in performances as was shown in the 1st implementation. This copy is made so that in case of write error (partial write), we always have a valid archive. This 1st implementation was given up, there is no trace of it.

## Consequences
* Faster operation than {simulation then zip} when the user wants their results as a single zip archive
* Cleaner code: all I/O error are handled in implementations of the interface, all directories are created in the corresponding implementation
* Output size divided by 5-10 when using the zip format

