# Commands

The Antares GUI gives access to a general menu of commands whose name and meanings are described hereafter.

## File

- **New** Create a new empty study to be defined entirely from scratch (network topology, interconnections
ratings, thermal power plants list, fuel costs, hydro inflows stats, wind speed stats, load profiles ,etc.)

- **Open** Load in memory data located in a specified Antares study folder. Once loaded, these data may be reviewed,
updated, deleted, and simulations may be performed. If "open" is performed while a study was already opened, the former study will be automatically closed.

- **Quick Open** Same action as open, with a direct access to the recently opened studies

- **Save** Save the current state of the study, if necessary by replacing original files by updated ones.
After using this command the original study is no longer available, though some original files may be kept until
the "clean" command is used (see "clean" command )

- **Save as** Save the current state of the study under a different name and / or location.
Using this command does not affect the original study. When "saving as", the user may choose whether
he/she prefers to save input and output data or only input data. Note that Antares does not perform "autosave":
Therefore, the actions performed on the input data during an Antares session (adding an interconnection,
removing a plant, etc.) will have no effect until either "save" or "save as" have been used

- **Export Map** Save a picture of the current map as a PNG, JPEG or SVG file. Default background color and
storage location can be changed

- **Open in Windows Explorer** Open the folder containing the study in a standard Windows Explorer window

- **Clean** Remove all junk files that may remain in the study folder if the Antares session has involved lots
of sequences such as "create area – add plant –save –rename area – save - rename plant ..."
(Antares performs only low level auto-clean for the sake of GUI's efficiency)

- **Close** Close the study folder. If no "save" or "save as" commands have been performed,
all the modifications made on the input data during the Antares session will be ignored

- **Quit** Exit from Antares

## Edit

- **Copy** Prepare a copy of elements selected on the current system map.
The command is available only if the current active tab (whose name appears at the top line of the subcommand menu)
is actually that of the System maps.

- **Paste** Paste elements previously prepared for copy. The command is available only if the current
active tab (whose name appears at the top line of the subcommand menu) is actually that of the System maps.
Note that copy/paste may be performed either within the same map or between two different maps, attached to
the same study or to different studies. To achieve that, launch one instance of Antares to open the "origin" study,
select elements on the map and perform copy, launch another instance of Antares to open the destination study,
perform paste. Copied elements are stored in an Antares clipboard that remains available for subsequent (multiple)
paste as long as the system map is used as active window.

- **Paste Special** Same as Paste, with a comprehensive set of parameterized actions (skip, merge, update, import)
that can be defined for each data cluster copied in the clipboard. This gives a high level of flexibility for
carrying out complex copy/paste actions.

- **Reverse** The elements currently selected on the system map are no longer selected and are replaced by
those not selected beforehand.

- **Unselect All** Unselect all elements currently selected on the system map.

- **Select All** Select all elements on the system map.

## Input

- **Name of the study** Give a reference name to the study. The default name is identical to that of
the study's folder but the user may modify it. The default name of a new study is "no title"

- **Author(s)** Set the study's author(s) name. Default value is "memory"

The other "input" subcommands here below are used to move from one active window to another.
Note that the availability of the __Wind__, __Solar__, and __Renewable__ subcommands depend on the advanced
parameter *"Renewable Generation modeling"* described in [miscellaneous](08-miscellaneous.md).

- **System Maps**
- **Simulation**
- **User's Notes**
- **Load**
- **Solar**
- **Wind**
- **Renewable**
- **Hydro**
- **Thermal**
- **Misc. Gen.**
- **Reserves/DSM**
- **Links**
- **Binding constraints**
- **Economic opt.**

## Output

**\<Simulation type\>\<simulation tag\>**

For each simulation run for which results have been generated, open a GUI for displaying results.
Results may be viewed by multiple selections made on a number of parameters. Note that, since all simulations do
not include all kinds of results (depending on user's choices), some parameters are not always visible.
Parameters stand as follows:

- Antares area (node)
- Antares interconnection (link)
- Class of Monte-Carlo results :
    - Monte-Carlo synthesis (throughout all years simulated)
    - Year-by-Year (detailed results for one specific year)
- Category of Monte-Carlo results :
    - General values (operating cost, generation breakdown, ...)
    - Thermal plants (detailed thermal generation breakdown)
    - Renewable generation (per cluster)
    - Record years (for each Antares variable, identification of the Monte-Carlo year for which lowest and highest values were encountered)

- Span of Monte-Carlo results :
    - _Hourly_
    - _Daily_
    - _Weekly_
    - _Monthly_
    - _Annual_

The interface provides a user-friendly way for the comparison of results between multiple simulations
(e.g. "before" and "after" commissioning of a new plant or interconnection):

- Use "new tab" button and choose a first set of simulation results
- Use again "new tab" and choose a second set of simulation results

The results window will be automatically split so as to show the two series of results in parallel.
To the right of the "new tab" button, a symbolic (icon) button gives further means to compare results on a
split window (average, differences, minimum, maximum and sum).

Besides, when the simulation results contain the "year-by-year" class, it is possible to carry out an
extraction query on any given specific variable (e.g. "monthly amounts of CO2 tons emitted") throughout
all available years of simulation.

The results of such queries are automatically stored within the output file structures, so as to
be available at very short notice if they have to be examined later in another session (extractions may require
a significant computer time when there are many Monte-Carlo years to process).

- **Open in Windows Explorer** This command displays the list of available simulation results and allows
browsing through the output files structure. The content of these files may be reviewed by tools such as Excel.
File structures are detailed in [Output Files](05-output_files.md).

## Run

- **Monte Carlo Simulation** Runs either an economy simulation, an adequacy simulation, or a "draft" simulation,
depending on the values of the parameters set in the "simulation" active window (see [Simulation window](04-active_windows.md#simulation)).
If hardware resources and simulation settings allow it, simulations benefit from full multi-threading
(see [System requirements](09-system_requirements.md))

- **Time-series Generators** Runs any or all of the Antares stochastic time-series generators,
depending on the values of the parameters set in the "simulation" active window (see [Simulation window](04-active_windows.md#simulation)), and
each cluster's "Generate TS" parameter (see [Thermal window](04-active_windows.md#thermal))

- **Time-series Analyzer** Runs the Antares historical time-series analyzer.
The parameters of this module are defined by a specific active window, available only on launching the analyzer
(see [Time-series analysis and generation](06-time_series_analysis_and_generation.md))

- **Kirchhoff's Constraints Generator** Runs the Antares Kirchhoff's Constraints Generator.
The parameters of this module are defined by a specific active window, available only on launching the KCG
(see [Kirchhoff Constraints Generator](07-kirchhoffs_constraint_generator.md))

## Configure

- **Thematic Trimming** Opens a window in which a choice can be made regarding the individual output status of
the variables defined in [Output Files](05-output_files.md). Each computed variable can either be stored as part of
the Output data to produce at the end of the simulation, or trimmed from it. In the latter case, the variable is regularly computed but the printing stage is skipped. Thematic Trimming does not reduce computation time but can bring some benefits on total runtime (smaller files to write). Thematic Trimming can save large amounts of storage space in simulations where only a handful of variables are of interest.

- **Geographic Trimming** Opens an auxiliary window that allows multiple selection of the results to store at
the end of a simulation: Choice of areas, interconnections, temporal aggregations (hourly, daily, etc.).
Note that in addition to this feature, alternative access to the function is available
(see [Active windows](04-active_windows.md), "output profile"). Geographic Trimming does not reduce actual computation
time but can bring some benefits on total runtime (fewer files to write). Geographic Trimming can save large
amounts of storage space in simulations where only a few Areas and Links are of interest.

- **Regional Districts** Allows selecting a set of areas to bundle them together in a "district".
These are used in the course of simulations to aggregate results over several areas.
They can be given almost any name (a "@" prefix is automatically added by Antares).
Bypassing the GUI is possible (see [Miscellaneous](08-miscellaneous.md)).

- **MC Scenario builder** For each Monte-Carlo year of the simulation defined in the "Simulation" window,
this command allows to state, for each kind of time-series, whether it should be randomly drawn from
the available set (be it ready-made or Antares-generated) _**OR**_ should take a user-defined value
(in the former case, the default "rand" value should be kept; in the latter, the value should be the reference number of the time-series to use). Multiple simulation profiles can be defined and archived. The default active profile gives the "rand" status for all time-series in all areas (full probabilistic simulation).

  Regarding Hydro time-series, the scenario builder gives, in addition to the assignment of a specific number to use for the inflows time-series, the ability to define the initial reservoir level to use for each MC year.

- **MC Scenario playlist** For each Monte-Carlo year of the simulation defined in the "Simulation" active window,
this command allows to state whether a MC year prepared for the simulation should be actually simulated or not.
This feature allows, for instance, to refine a previous simulation by excluding a small number of "raw" MC years
whose detailed analysis may have shown that they were not physically realistic. A different typical use consists
in replaying only a small number of years of specific interest (for instance, years in the course of which Min or Max
values of a given variable were encountered in a previous simulation).

  In addition, each MC year i=1, …, N can be given a relative “weight” \\(W_i\\)  in the simulation (default value: 1). The expectation and standard deviation of all random variables will then be computed as if the scenarios simulated were sampled from a probability density function in which MC year i is given the probability

  $$\frac{W_{i}}{\sum_{j=1,...,N}{W_{j}}}$$

- **Optimization preferences** Defines a set of options related to the optimization core used in the simulations.
The set of preferences is study-specific; it can be changed at any time and saved along with study data.
Options refer to objects (binding constraints, etc.) that are presented in subsequent sections of this document.
The values set in this menu overlay the local parameters but do not change their value: for instance, if the LOCAL
parameter "set to infinite" is activated for some interconnections, and if the GLOBAL preference regarding transmission
capacities is "set to null", the simulation will be carried out as if there were no longer any grid BUT the local
values will remain untouched. If the preference is afterwards set to "local values", the interconnections will be
given back their regular capacities (infinite for those being set on "set to infinite").

    - _Binding constraints (include / ignore)_
    - _Hurdle costs (include / ignore)_
    - _Transmission capacities (local values / set to null / set to infinite)_
    - _Min Up/down time of thermal plants (include / ignore)_
    - _Day-ahead reserve (include / ignore)_
    - _Primary reserve (include / ignore)_
    - _Strategic reserve (include / ignore)_
    - _Spinning reserve (include / ignore)_
    - _Export mps (false/true)_
    - _Simplex optimization range [^4] (day / week)_
    - _Unfeasible problems behavior (Error Dry/ Error Verbose/ Warning Dry/ Warning Verbose_

- **Adequacy Patch** Auxiliary window [Options] Defines a set of options related to the adequacy patch.
The set of preferences is study-specific; it can be changed at any time and saved along with study data.
Auxiliary window [Areas] Opens a window in which a choice can be made regarding the individual area adequacy patch mode.

- _Enable Adequacy patch (false / true)_
- _NTC from physical areas outside to physical areas inside adequacy patch (set to null / local values)_
- _NTC between physical areas outside adequacy patch (set to null / local values)_
- _Price taking order (DENS / Load)_
- _Include hurdle cost in CSR optimization (false / true)_
- _Check CSR cost function value prior and after CSR (false / true)_
- _Thresholds:_
    - _Initiate curtailment sharing rule_
    - _Display local maching rule violations_
    - _Relax CSR variable boundaries_

- **Advanced parameters** Advanced Parameters allow to adjust the simulation behavior regarding issues
that are more numerical than physical. The set of parameters is study-specific and can be updated at any time.

    - Seeds for random number generation
      - Time-series draws (MC scenario builder)
      - Wind time-series generation
      - Solar time-series generation
      - Hydro time - series generation
      - Load time - series generation
      - Thermal time-series generation
      - Noise on thermal plants costs
      - Noise on unsupplied  energy  costs
      - Noise on spilled energy costs
      - Noise on virtual hydro cost
      - Initial hydro reservoir levels
    - Spatial time-series correlation
      - Numeric Quality : load `[standard | high]`
      - Numeric Quality : wind `[standard | high]`
      - Numeric Quality : solar `[standard | high]`
    - Other preferences
      - Reservoir Level Initialization `[cold start | hot start]`
      - Hydro Heuristic policy `[accomodate rule curves | maximize generation]`
      - Hydro Pricing mode `[fast|accurate]`
      - Power fluctuations `[free modulations | minimize excursions | minimize ramping]`
      - Shedding policy `[shave peaks | minimize duration]`
      - District marginal prices : `[average | weighed]`
      - Day-ahead reserve management `[global | local]`
      - Unit commitment mode `[fast | accurate]`
      - Simulation cores `[minimum | low | medium | high | maximum]`
      - Renewable Generation modeling `[aggregated | cluster]`

## Tools

- **Study manager** Launches the "study manager" external package (Please refer to dedicated documentation
for this package)

- **Resources monitor** Indicates the amounts of RAM and disk space currently used and those required for a simulation
in the available modes (see [System requirements](09-system_requirements.md)).
Note that the "disk requirement" amount does not include the footprint of the specific "mps" files that may have
to be written aside from the regular output (see previous § "optimization preferences").
Besides, the resources monitor shows the number of CPU cores available on the machine Antares is running on.

- **Configure the temporary folder** Defines the location that will be used by Antares to store the temporary files of
the MC simulators. This location is used behind the scene by some Antares Simulator components (for instance scenario
builder or time-series analyzer). The default setting is the system temporary folder.

## Window

- **Toggle full window** Uses the whole window for display

- **Inspector** Opens a window that gives general information on the study and allow quick browsing
through various area- or interconnection-related parameters. The Inspector window displays the content of the
Antares clipboard, e.g. areas and interconnections selected on the current study map. If the "Geographic Trimming"
option of the general simulation dashboard has the value "custom", the filtering parameters can be defined within
the Inspector window. Besides, areas currently selected, displayed in the Inspector window,
can be bundled into an "output district" by using the Configure/district command previously described

- **Log viewer** Displays the log files regarding every Antares session performed on the study


[^4]:_Weekly optimization performs a more refined unit commitment, especially when the level selected in the "advanced parameters" menu is "accurate"._


