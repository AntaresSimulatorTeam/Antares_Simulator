# Output files

_**This section is under construction**_

1. The outputs of the Antares time-series analyzer are not printed in the general output files but kept within
   the input files structure (the reason being that they are input data for the Antares time - series generators).
   The associated data may nonetheless be accessed to be reviewed, updated and deleted at any time through the GUI.
2. The times-series analyzer requires the use of a temporary directory in which intermediate files are created
   in the course of the analysis and deleted in the end, unless the user wishes to keep them for further examination.
   Its location is user-defined and should usually be the "user" subfolder if files are to be kept, otherwise any
   proper temporary space such as `C..../Temp`.

## Scenario mapping file
"scenariobuilder.dat"  

**MC Scenario builder** For each Monte-Carlo year of the simulation defined in the "Simulation" window,
this command allows to state, for each kind of time-series, whether it should be randomly drawn from
the available set (be it ready-made or Antares-generated) _**OR**_ should take a user-defined value
(in the former case, the default "rand" value should be kept; in the latter, the value should be the reference number
of the time-series to use). Multiple simulation profiles can be defined and archived.
The default active profile gives the "rand" status for all time-series in all areas (full probabilistic simulation).

Regarding Hydro time-series, the scenario builder gives, in addition to the assignment of a specific number to use
for the inflows time-series, the ability to define the initial reservoir level to use for each MC year, also hydro
max power scenario builder is available to support time-series for Maximum Generation and Maximum Pumping because
the number of TS's for ROR, Hydro Storage and Minimum Generation can be different than the  number of TS's for
Maximum Generation and Maximum Pumping.