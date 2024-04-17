# Active windows

[//]: # (TODO: fetch important content & remove this page)

## Simulation

The main simulation window is divided up in two parts. On the left side are the general parameters while the right side is devoted to the time-series management.

These two parts are detailed hereafter.


### RIGHT PART: Time-series management

For the different kinds of time-series that Antares manages in a non-deterministic way (load, thermal generation, 
hydro power, hydro max power, wind power, solar power or renewable depending on the option chosen):

1. **Choice of the kind of time-series to use**
Either « ready-made » or «stochastic » (i.e. Antares-generated), defined by setting the value to either "on" or "off". 
2. Exception is hydro max power that can only be « ready-made ». Note that for Thermal time-series, the cluster-wise 
3. parameter may overrule this global parameter (see Thermal window description below).

2. **For stochastic TS only**:

    - **Seasonal correlation** ("monthly" or "annual") Indicates whether the spatial correlation matrices to use are 
   defined month by month or if a single annual matrix for the whole year should rather be used (see [Time-series analysis and generation](06-time_series_analysis_and_generation.md))

    - **Store in output**
        - **Yes**: the generated times-series will be stored as part of the simulation results
        - **No**: no storage of the generated time-series in the results directories

3. **General rules for building up the MC years**
    - **Intra-modal**:
        - **Yes** For each mode, the same number should be used for all locations (or 1 where there is only one TS), 
        but this number may differ from one mode to another. For instance, solar power TS = 12 for all areas, while wind power TS number = 7 for all areas.
        - **No** Independent draws
    - **Inter-modal**:
           - **Yes** For all modes, the same number should be used but may depend on the location (for instance, solar 
           and wind power TS = 3 for area 1, 8 for area 2, 4 for area 3, etc.)
           - **No** Independent draws


