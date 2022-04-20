Antares Changelog
=================
v8.2.0 (03/2022)
--------------------
### New features
- Multiple timeseries for link capacities (NTC). It is now possible to establish different scenarios for the capacity of a link. Users can now take partial or total outages for links into account #520
- Infeasible problem analyzer. When the underlying problem has no solution, list the most suspicious constraints in a report. This should help users identify faulty binding constraints #431
- Add a hydro-debug switch that allows the printing of some useful debug data in heuristic mode #254

### GUI
- Add a "view results" button in the dialog that appears when a simulation has been completed #511
- Help menu : add an "online documentation" item #509
- Improve UI for new thermal parameter "tsGenBehavior" #534
- Improve cell styles when loop-flow is enabled for a link #571

### Bug fixes
- Prevent an area from having a link to itself #531
- Fix crash when the study folder does not exist #521
- Fix crash when failing to load a study #502

### For developers
- Remove calls to exit() #505. Provide consistent return values for antares-solver by fixing a segfault related to the log object #522
- Remove calls to setjmp, goto's big brother #527
- Large refactor of antares-solver's main function, hoping to make error management easier to understand #521
- Use std::shared_ptr instead of Yuni::SmartPtr in most cases #529
- Simplify and clean some parts of the code #537, #538, #540

v8.1.1 (01/2022)
--------------------

### Bug fixes
- Fix segfault occurring randomly when thermal clusters are disabled (#472)
- Fix hydro level discontinuities (#491). Very rarely, hydro reservoirs would inexplicably be filled from 0% to 100% in 1h, this violating the modelling constraints.
- Execution times when the Sirius solver is used in conjunction with OR-Tools are now similar as with Sirius alone. This is a result of [this fix](https://github.com/AntaresSimulatorTeam/or-tools/pull/1), related to "hot-start".

### GUI
- Speed up scrolling (#395)
- Warn the user about disabled renewable clusters only when relevant (#386)

### Packages
Include antares-analyzer into .zip and .tar.gz archives (#470). This is especially useful if you use the portable version of Antares.

### For developers
In the CI workflow, CentOS 7 now uses devtoolset-9 (previously 7), with in particular gcc 9. This allows for C++17 features to be used without hassle.


v8.1.0 (09/2021)
--------------------

### New features
- Allow up to 9 RES groups (off-shore wind, on-shore wind, rooftop solar, PV solar, etc.) as opposed to wind and solar previously. This allows the user to distinguish between more renewable energy sources. When creating a new study, renewable generation modelling is set to "clusters" by default. This change does not affect opening an existing study. Note that TS generation is not available for these new RES groups.
- Add 3 thermal groups, named other, other 2, other 3 and other 4.

### Bug fixes
- When a binding constraint is marked as skipped in the GUI, disable it in the solver #366

### GUI
- Keep selection on thermal/renewable cluster when its group changes #360
- Dialogs "Thematic trimming" and "User playlist" are now resizable

### For developers
- Add non-regression tests on each release
- Fix vcpkg on Github Actions
- Add build cache for Github Actions to speed up the build (Linux only)

v8.0.3 (05/2021)
--------------------

### Bug fixes

- Fix calculation of average for variable "congestion probability"
- Fix NODU when unit number is not an integer i.e has decimals
- GUI: allow decimal nominal capacity for thermal clusters
- GUI: Linux: use xdg-open to open pdf files instead of gnome-open

### For developers

- Remove code related to licence management
- Remove openssl and libcurl dependencies
- Remove dead code

v8.0.2 (04/2021)
-------------------- 

### Bug fixes

- Fix GUI freeze when area color is changed but user don't validate new color
- Correction of MC year weight use for PSP and MISC NDG

v8.0.1 (03/2021)
-------------------- 
### Features

- Add "Continue Offline" button at startup if antares metric server is unreachable

### Bug fixes

- Error with hydro start when using scenario playlist and stochastic TS refresh span
- Files needed for antares-xpansion not exported when using scenario playlist with first year disabled
- Correction of crash if user define a stochastic TS refresh span of 0 : minimum value is now 0
- Correction of MC years playlist weight write when sum of weight was equal to number oy years (no MC years playlist export in .ini)

### For developers

- Add a GitHub action to check if branch name will launch CI
- Add shared dll in windows .zip archive

v8.0.0 (03/2021)
-------------------- 

### Features

- OR-Tools integration :
    - add command line option in antares-solver to define OR-Tools use and OR-Tools solver (option --use-ortools and --ortools-solver='solver')
    - add GUI option in run simulation to define antares-solver launch with OR-Tools option

- Add advanced hydro allocation feature. The default and existing behavior is to accomodate the guide curves, the new behavior is to maximize generation, even if it means that the reservoir level goes beyond the guide curves.

- Add indication on how to disable anonymous metrics

- antares-xpansion :
    - add option `include-exportstructure` in `generaldata.ini` to export .txt files needed for antares-xpansion

- Scenario builder and hydraulic level :<br/>
  Adding an hydraulic starting level tab in the scenario builder.<br/>
  For each MC year and area, a starting level can be defined, that is a 0-100 value.<br/>
  When the scenario builder is enabled, these levels get priority upon hot-start mode.

- Binding constraints (BC) and thermal clusters :<br/>
  If a must-run or disabled cluster is involved in a binding constraint :
    - the cluster is marked as "N/A" in the BC formula (GUI > Binding constraint > Summary)
    - the cluster is marked as must-run/disabled in the Weights or Offsets tabs.

  If a BC involves only zero weighted clusters/links or must-run/disabled clusters, the BC is :
    - marked with a red bullet in the Summary tab
    - marked as Skipped in the Weights and Offsets tabs

- MC Scenario Playlist :
  Add possibility to define a weight for each MC years in the synthetis results.<br/>
  See : GUI > Configure > MC scenario playlist.<br/>
  By default, a MC year's weight is 1, but can be set by user to more or less.<br/>
  After simulation, the MC year have a contribution to averages or standard deviations in synthesis results
  depending on the weight it was given.

### Bug fixes

- Selecting an area and then, from the inspector, trying to select a thermal cluster or a link of this area in the dependencies
  section causes a crash. The inspector's cluster/link selection was removed.
- Scenario builder :
    - It makes no sense for the user to access the scenario builder Configure menu item whereas the Building mode parameter is set
      to Automatic or Derated. In the previous cases, the Configute menu item is disabled.
    - If a disabled thermal cluster is given a time series number in a non active rule of the scenario builder, a warning should not be
      triggered. If the disabled cluster is given a number for many MC years in the active rule, a single summary warning should be raised,
      not a warning per year.

### For developers

- External dependencies :
    - use of new repository [antares-deps](https://github.com/AntaresSimulatorTeam/antares-deps) for external dependencies compilation

- Fix several compilation warnings
- Remove unused `COUT_TRANSPORT` constant
- Add code formatting with clang-format
- Remove PNE dead code

- docker image :
    - create of dockerfile for docker image creation

- continuous integration :
    - use docker images in CI
    - use of antares-deps release artifact in CI
    - push of docker image to dockerHub in [antaresrte/rte-antares repository](https://hub.docker.com/repository/docker/antaresrte/rte-antares)
    - add Centos7 support

- Unit tests :
    - Adding an end-to-end test in memory (see simple-study.cpp) :<br/>
      This test calls high level functions to build a simple study and runs it.<br/>
      It then checks if some elements of results match associated expected values.<br/>
      During this process, file system is not involved : everything takes place in RAM
    - Adding pytest scripts to check reference output values
    - Add pytest scripts related to unfeasible problems

v7.2.0 (06/2020)
-------------------- 

### Features

- Simulation dashboard: A new  "Geographic Trimming" option
  is now available in the "Configure" menu. This option makes
  it possible to  filter the simulation's output content so as
  to include only results regarding Areas and Links of interest

- Optimization:  a new parameter "Unfeasible Problems Behavior"
  is available in the "advanced preferences" section of the
  "Configure" menu, with four possible values:
  (Error Dry, Error Verbose, Warning Dry, Warning Verbose)
  The first two options make the simulation stop right after
  encountering the first mathematically unfeasible problem, if any
  The last two options make the simulation skip all unfeasible
  problems, if any  
  "Verbose" options print faulty problems in the “mps” format
  "Dry" options only report the time frame (MC year, week) for which
  an unfeasible problem was detected

- Compilation and cmake tree :
  Updates were made for more modern CMake use.
  Git submodules (extern dependencies : curl, openssl, wxwidget) are no more in use.
  These external dependencies can be retrieved :
    - either from a library manager : vcpkg for Windows, classic package
      repositories for Linux. With this way to proceed, an installation of external
      dependencies is required once for all.
    - or thanks to an automatic download : at Antares' cmake configure step,
      all needed downloads, compilation and installation are done.

- Unit tests :
  unit tests around class Matrix are now available.
  They can be compiled (on demand) during Antares' cmake build step
  and run either with ctest or in the classic way.
  Boost.Test is required and can be priorily retrieved and installed in the
  same way as the other external dependencies.

- Continuous integration : yaml files for github actions allow the run of
  all build chain and unit tests on several environment (Windows and Ubuntu).
  The 2 ways of getting external dependencies are also tested.

- Documentation: updated reference guide

- Usage metrics: added reference key for this version

### Bug fixes

- GUI of the "Thematic trimming" option: Window size is naturally readjusted
  to improve readability by upgrading wxwidgets (3.1.3 and above).

- Auxiliary "Batchrun" tool: two options previously missing in the
  command line syntax have been introduced and now make it possible
  to launch a sequence of simulations to run in parallel

v7.1.0 (12/2019)
-------------------- 

### Features

- Simulation Dashboard: A new option "Thematic Trimming"
  is available in the "Output Profile" Section. This option
  now makes it possible to define precisely the content of
  output files so as to include only variables of interest

- Optimization: a new parameter "Hydro Pricing mode" is   
  available in the "advanced parameters" section, with two
  possible values (fast, accurate):
  In mode "fast", water value is, in the course of optimization,
  taken to be constant throughout the (daily or weekly)
  optimization period, and equal to that found for the exact
  day and level at which the optimization begins. Water values
  are reassessed afterwards, for each hour, on the basis of
  relevant time and level.
  In mode "accurate", the variations of water value along with
  the reservoir level are taken into account in the course of
  the (weekly) optimization. Reference (level-dependent) values
  are those attached to the end of the week. Water values
  are reassessed afterwards, for each hour, on the basis of
  relevant time and level.

- Documentation: updated reference guide

- Documentation: updated optimization problem formulation
  (modelling of hydro pricing options)

- Usage metrics: added reference key for this version

### Bug fixes

- Output file "mc-all/grid/digest.txt": replaced "NaN" values
  by zeroes, where appropriate

- Output file "mc-all/grid/digest.txt": replaced "0" values
  by N/A, where appropriate (especially, hydro reservoir-related
  variables, when the "reservoir management" area attribute is set
  to "No")

- Output GUI: fixed a display bug regarding missing items in the
  "links" panel, in the case where simulation parameters are set
  so as not to produce synthetic results

- Links GUI: improved integrity control regarding hurdle costs.
  Negative values are allowed in either direct or indirect
  orientation, provided that the sum of both is non-negative

- General GUI: removed redundant items and renamed option menu
  "Geographic District" as "Regional District" to avoid confusion
  with new "Trimming" options

- Output: when simulation results are trimmed so as not to produce
  any data for given Areas or Links, avoid creation of empty folders
  named after said Areas or Links

v7.0.1 (04/2019)
-------------------- 

### Features

- Time-series analysis: in "detrended mode", extended perimeter
  to raw data including periods with no meaningful signal
  (e.g. solar production at night)
- Hydro-storage modelling: added ability to optimize pumping along
  with generation in mode "use heuristic target without leeway"

v7.0.0 (12/2018)
-------------------- 

### Features

- GPL release: updated companion files (README,...)
- GPL release: updated project Icons
- GPL release: insertion of license headers
- GPL release: translation of comments
- GPL release: removal of license control
- GPL release: code restructuring to separate Antares and Sirius
- Examples library: upgraded and added 16 new examples
- Documentation: updated reference guide
- Documentation: updated map editor guide
- Documentation: updated optimization problem formulation
- Documentation: updated examples library

v7.0.0-rc (12/2018)
-------------------- 

### Features

- Improved code for linux compilation with gcc 7

### Bugs

- Fixed various issues in GUI
- Fixed RHS of constraints generated by the KCG when
  min and max values of PST settings are strictly equal
  and constraints are generated for the whole year

v6.5.1 (11/2018)
---------------- 

### Bugs

- Fixed index in hydro heuristic engine
- Hydro GUI: added scrollbars for correct display on laptops
- Output: improved presentation of results for incomplete calendar-based weeks
- Kirchhoff's constraint generator: fixed several GUI issues
- Districts GUI: improved syntax control

v6.5.0 (11/2018)
---------------- 

### Features

- Implementation of Kirchhoff's laws (DC approximation),
  modeling of phase-shifters and representation of passive
  loop flows (to account for on highly reduced gris): a
  dedicated Kirchhoff's constraints generator is now available
  It makes use of both classical input data (impedances)
  and new input data. Its results are specific binding
  constraints whose names begin by @UTO-, storable in the
  INPUT folder after user's validation ("save")

  New or modified input data for link L (8760 hourly values):
  Impedances (moved from col.3 to col.5)(Ohms at ref. voltage U)
  Loop flow (passive) (MW)
  Min Tap of phase-shifter  (MW*Ohms/U2 along any AC cycle including L)
  Max Tap of phase-shifter  (MW*Ohms/U2 along any AC cycle including L)		
  New link parameters (one value)		
  Asset type (AC,DC,Gas,Virtual,Other) : KCG deals only with AC links
  "account for loop flow" toggle
  "tune PST"		toggle
  KCG generating directives:
  Working map to use for generation
  Calendar to use for constraints activation (relaxation outside)
  Status of passive loop flow in constraints RHS (included or not)
  Status or PST settings in constraints RHS      (included or not)
  Auto-check of nodal loop flow balance (activated or not)
  Definition of the "infinite" to use for constraints relaxation  
  KCG results:
  For AC Links involved in the generation process: The KCG sets the
  values of the two input data toggles related to loop flows and
  PST settings, in accordance with the current generation directives

  		Identification of an optimal (minimum-weight) cycle basis for the
  		formulation of constraints 
  		
  		Generation of all relevant constraints (equality, inequalities, with
  		or without relaxation) 

- Reservoir-type hydro and other energy storage facilities:
  interface, input and output data structure, functionalities,
  have been completely redesigned. As a consequence, a number
  of new items (variables & parameters) are introduced in both
  input and output, while a few input variables are redefined
  or deprecated:

  Deprecated hydro variables:
  Pmax hydro "min", Pmax hydro "max"
  Redefined hydro variables and parameters:
  Hydro-storage time-series :    redefined at the daily scale
  Bounds for Reservoir levels:   redefined at the daily scale
  Res.level initialization date: redefined at the monthly scale
  New hydro variables and parameters:
  Input : max daily hydro generating energy
  max daily hydro pumping	energy and power
  monthly-to-daily  inflow breakdown pattern
  water value (time, level)
  modulation of max generating power (level)
  modulation of max pumping power    (level)
  pumping efficiency
  +many "storage management options" parameters
  Output: Reservoir level 	(H.LEV)
  Water value 		(H.VAL)
  Pumping power		(H.PUMP)
  Natural Inflow		(H.INFL)
  Forced Overflow		(H.OVFL)
  Cost of Gen+Pumping (H.COST)
  Optimization preferences:
  "Hot/Cold start" (year N may start or not at the final N-1 level)

- GUI: Districts may now be defined from within the interface
  (notepad tab connected to the Inspector's clipboard)

- Time-series generation (solar, wind, load) : increased speed
  when "high accuracy" option is selected, in the special case
  where all diffusion processes produce "Normal" variables

- Example library: upgraded to 6.5 (without extension)

### Bug fixes

- Time-series generation: the storage (Input folder)
  of time-series generated for thermal clusters either in the
  "disabled" or "must-run" state did not work properly

- Time-series analysis: when short- and long-term levels
  defined for auto-correlation assessment are identical, the
  analyzer now performs a pure exponential fitting

- Time-series analysis: monthly time-series containing no
  non-zero value are no longer rejected by the analyzer

- Output: the link-variable "MARG.COST" was rounded to an integer
  value (changed to 2 decimal accuracy)


v6.1.3 (06/2018)
---------------- 

### Features

- Output: added a new file at the root of simulation results,
  displaying a short summary of the overall system economic
  performance throughout all Monte-Carlo years

- Log file: added new info messages on the size of optimization
  problems

- Updater (standalone): added new options and improved
  help messages

- Expansion mode: presolve stage replaced by hot start

### Bug fixes

- Simulation: In the "accurate" Unit Commitment mode, the
  optimization preference "thermal Clusters Min Up/Down Time"
  can now be turned to "ignore"

- Simulation: removed remaining debug traces

- Simulation: zero-reset on interconnection marginal costs
  was sometimes missing in optimization final stage

- Example library : upgraded to 6.1 and extended


v6.1.2 (11/2017)
---------------- 

### Features

- Solver, Simplexe package: Improvement of the Scaling stage
  (Matrix, right hand side, costs)


v6.1.1 (11/2017)
---------------- 

### Features

- Solver: Light changes in Presolve stage


v6.1.0 (09/2017)
---------------- 

### Features

- GUI and simulation: "binding constraints" objects may now involve
  not only flows on interconnections but also power generated from
  thermal clusters. Alike flows, generation from thermal clusters may
  be handled either on an hourly, daily or weekly basis and  may be
  associated with arbitrary offsets (time-lags expressed in hours).


v6.0.6 (07/2017)
---------------- 

### Features

- GUI: Binding constraint parameters tables (weights and offsets) are trimmed
  line-wise so as to fit exactly with the content of the selected working map

- Solver: strenghtening of the final admissibility check step in the "accurate"
  commitment mode



v6.0.5 (07/2017)
---------------- 

### Bug fixes

- Solver: Pruning of redundant messages in simulations launched from command line

- Solver: Removal of misprints in command line help messages

- Files:  Fixed issues (detected as of 6.0.1) regarding storage of thermal time-series files

- Study Cleaner: Unwarranted removal of the graphic multi-map lay-out could occur when
  cleaning datasets  (detected as of 6.0.0)


v6.0.4 (06/2017)
---------------- 

### Bug fixes

- GUI: The "variable per variable" view of the output files allows
  to display the power generated by each thermal cluster

- Simulation: Negative "ROW Balance" is properly included in
  unsupplied energy allowances


v6.0.3 (06/2017)
---------------- 

### Features

- GUI: The number of system maps that could be stored in a given study
  was limited to 19. This number is now unbounded.

### Bug fixes

- GUI: The list of thermal clusters displayed for a given Area in the
  current map was sometimes wrongly initialized (Area considered
  selected though not explicitly clicked on yet)

-  GUI: The order in which binding constraint terms are shown in the
   "summary" Window could depend on the execution platform used

-  GUI: The Antares study icon could not be properly copied in some
   circumstances


v6.0.2 (06/2017)
---------------- 

### Features

- Optimization : To help discriminate between equivalent economic
  solutions, random noises on hydro hourly prices are more regularly
  spread out (absolute values) in the interval (5 e-4 ,1 e-3)Euros/MWh

### Bug fixes

- Simulation : The identification of the Monte-Carlo year numbers
  in which the smallest/greatest values of random variables are
  reached could be ambiguous when identical results are found for
  two years ore more.


v6.0.1 (05/2017)
---------------- 

### Features

- Thermal Time-series generation: Data regarding all thermal clusters
  are generated and stored in the same way, regardless of their activity
  status (unabled/disabled). This makes easier to check data consistency

- Simulation: Upper bounds for spilled power and unsupplied power	are
  actually set to their maximum theoretical value(i.e. if economic
  conditions make it justified: spill all power or shed all demand)
  So far, spillage of power that could be absorbed by the local demand
  was not allowed

- Simulation: a silent "Expansion" mode has been added to the regular
  modes "Economy/Adequacy/Draft". The three differences with the
  "Economy" mode are:
  a) In "accurate" unit commitment, integrity constraints are relaxed
  in the core optimization problem.
  b) Day-ahead reserve is no more subtracted from the initial demand
  to get back to "standard" conditions
  c) The values of all optimal criteria are printed in ad hoc files
  The use of this mode should be restricted to well-designed scripted
  automatic simulation sequences taking into account the simplifications
  listed above


v6.0.0 (04/2017)
---------------- 

### Features

- GUI: A new interface makes it possible to define several views (maps) of
  the Power System modelled in an Antares study. These maps are meant to give
  the user the ability to set different layouts in which each Antares Area
  or Link can be either shown or remain hidden. Accordingly, all input and
  output data windows can now adapt the information displayed so as to match
  exactly the content of any given map. Copy/Paste functions have been
  extended so as to work between different maps of different studies opened
  in multiple Antares sessions

- Simulation: Introduction of a flexible multi-threaded mode for the processing
  of  heavy problems: Antares "Monte-Carlo years" can be be distributed on a
  number of CPU cores freely set by the user. This parameter appears as a new
  tunable item of the  "advanced parameters" list  attached to any Antares Study.
  Five values are available in the [1, N] interval,  N being the number of CPU
  cores of the machine (virtual or physical) Antares is run on

- License control through the internet: a new system has been developed for
  accommodating situations where users wish to operate Antares on a large
  fleet of machines among which a limited set of commercial license tokens
  can float freely

- Data organizer: Antares studies often include a great number of files of
  all sizes, which may take long to process when multiple copies are needed.
  Likewise, the management of the HDD space required for regular storage of
  all of the studies involved in a complex study workflow may turn out to be
  a demanding and heavy task. To save both time and hardware resources, the
  Antares Data Organizer, now provided as a companion tool to the Antares
  Simulator, brings the ability to schedule basic data management tasks
  such as study archiving/expansion (use of a specific compressed format),
  copy to backup folders, registering of studies and archives in catalogues.


v5.0.9-SE (04/2017)
---------------- 

### Bug fixes

- Random noises on thermal clusters costs now include the zero-cost
  "must-run" clusters (as a consequence, noises assumptions do not vary
  with the cluster status)

- Fixing an initialization issue that could sporadically affect the
  minimum number of committed thermal units (+1 or -1 deviation,
  "accurate" mode only)

v5.0.7-SE (04/2017)
---------------- 

### Features

- License control : management of SSL certificates encrypted through SHA-256 algorithm


v5.0.7 (12/2016)
---------------- 

### Bug fixes

- Fixing a packaging error


v5.0.6 (12/2016)
---------------- 

### Bug fixes

- Results processing: For full "must-run" thermal clusters, the NODU variable
  could be wrongly assessed in the "accurate" unit commitment simulation mode

- GUI: when the scenario builder feature is active, saving right after deleting
  a thermal cluster could result in a partial dataset corruption (references to
  the deleted object were kept alive in the scenario builder context)


### Features

- Unsupplied energy control: if the actual economic optimization requires it, load
  shedding is now allowed to occur in areas where the available thermal generation
  is higher than the local demand (e.g. if local VOLL < local thermal costs)

- Linear solver, hot starting of weekly problems: in the "fast" unit commitment
  mode, optimal bases are flushed at the beginning of each Monte-Carlo year. This
  comes as a pre-requirement for the next versions of Antares, which will be
  fully multi-threaded

- Simulation results: code segments processing all variables attached to spatial
  aggregates, and the variable representing the number of running thermal units
  on the first hour of the year, were re-written to be compatible with the next
  versions of Antares, which will be fully multi-threaded



v5.0.5 (08/2016)
---------------- 

### Bug fixes

- No-Load Heat costs and  Start-up costs: in the "fast" unit commitment options,
  the result was slightly below the actual optimal possible cost for some
  datasets (i.e. datasets in which the thermal cluster coming last in alphabetic
  order had a minimum stable power equal to zero).

- Spilled energy control: the three parameters defining how energy in excess should
  be split between the different possible sources when there is a choice to make
  can work properly again (feature inhibited in previous 5.0.x versions)


### Features

- License control throughout the internet: all combinations of UTF8 characters can
  now be used within proxy ids and passwords

- Economic optimization: in an area where the amount of available thermal power
  exceeds that of load, the fact that the demand should necessarily be served
  is locally expressed as a constraint of the optimization problem  (LOLE=0)


v5.0.4 (05/2016)
---------------- 

### Bug fixes

- Errors occured on loading the "min gen modulation" time-series of thermal clusters

### Features

- Better estimate of the number of thermal units dispatched in "fast" unit commitment mode
- Nodal Marginal Prices and Marginal yield on interconnections are now available in
  "accurate" unit commitment mode
- Binding constraints including offset parameters: unbounded positive or
  negative values can be used for all classes of constraints (hourly, daily, weekly)


v5.0.3 (05/2016)
---------------- 

### Bug fixes

- Crashes occured when the "full must-run status" parameter was set on
  "true" for thermal clusters


v5.0.2 (04/2016)
---------------- 

### Bug fixes

- Removed debug information that should not be displayed in release mode

### Features

- The optimization criterion used to assess the hydro energies to generate throughout
  each month incorporates heavier penalization terms for the 12 deviations from the
  theoretical monthly targets (formerly, only the largest deviation was penalized).


v5.0.1 (04/2016)
---------------- 

### Bug fixes

- Adequacy mode: fixed a memory allocation bug that forced the post-simulation
  output files processing to be interrupted

- In the previous version, additional logs were added. That could lower the simulation
  performances in some cases. This problem is now solved.


v5.0.0 (03/2016)
---------------- 

### Bug fixes

- GUI, system map:  copy /paste of binding constraints could alter the activity status or
  the names of the duplicated binding constraints in some instances

- GUI, system map:  some conflicts in copy/paste actions were not always properly raised
  (e.g. attempt to copy three nodes and paste them on two other nodes)

- Thermal clusters: Improved checking of time-series generation parameters (improper use of a
  nominal capacity modulation factor lower than the minimum stable power is no longer possible)

- Thermal clusters: Improved checking of ready-made time-series. If the user-chosen time-series
  are not consistent with the parameters set in the GUI, warnings are issued in log files

- Output , LOLD variable:  in some instances, the values assessed in "economic" simulation mode and in
  "adequacy" simulation mode could slightly differ because of sporadic rounding side-effects.
  rounding convention is now set uniformly to : 0 < X < 0.5 -> (X=0)

- Output, MISC.NDG and PSP variable:  values were not properly edited for the specific category
  "geographic districts, "year-by-year results"

- Output, OV. COST, OP. COST, NP. COST variables: values were not properly edited for the last
  hour of the last day of the simulation

- Output, File comparison functions: calendar marks were not properly displayed in some views

- Output, File comparison functions: "Max" operator has been added


### Features

- Optimization: introduction of a new unit-commitment mode based on a MILP approach slower but more
  accurate than the former one. An option lets the user choose which mode should be used (fast/accurate)

- Optimization: in "accurate" unit-commitment mode, incorporation of thermal start-up costs and
  no-load heat costs within the global objective function to minimize. In "fast" unit-commitment
  mode, start-up costs and no-load heat costs are minimized independently from the main objective

- Optimization: in both unit-commitment modes, improvement of the inter-weekly start-up strategies
  (seamless reformulation of the optimization results obtained beforehand)

- Thermal clusters: definition of separate minimum up/down durations to be used for unit-commitment

- Thermal clusters: definition of a minimum amount of power (hourly time-series) to be generated
  by the units of the cluster, regardless of economic considerations (partial must-run commitment)

- Thermal clusters: start-up cost can now be set from -5000000 to 5000000 (was from -50000 to 50000)

- Binding constraints: introduction of new "offset" parameters which make it possible to define
  constraints whose terms can refer to different times (e.g. 2 X(t) - 1.5 Y(t-4) + 3 Z(t+1) <10)

- Benchmarking: so as to allow transparent comparisons with other software, the user may demand
  that all optimization problems solved by Antares be printed in a standardized "mps" format
  along with the values of the optimized criterion.

- GUI, System map : new button available in the tool bar for centring the map on a (x,y) location

- GUI, System map : new button available in the tool bar for map trimming around used space

- Output: In synthetic Monte-Carlo results,year-by-year results and cluster-by-cluster results,
  Addition of a field "Number of dispatched units" (NODU)



v4.5.4 (03/2015)
----------------

### Bug fixes

- License checking: internet proxys for which no login and/or password have been
  defined can now be used

- Upgrade to 4.5 format of datasets edited in 4.4 format or lower, in which the "scenario builder"
  feature was activated: the conversion to 4.5 format could fail sometimes.

v4.5.3 (02/2015)
----------------

### Features

- Start-up and fixed thermal costs: the interpretation of the unit-commitment strategy
  (starting-up and shutting-down times of each thermal unit) includes the explicit
  minimization of the total sum of start-up costs and fixed costs (in previous versions,
  units were called on as late as possible and called off as soon as possible)


- Various improvements in the linear solver yielding some speed increase in hard cases


- Control of license validity through the internet (setting up of a dedicated server)


### Bug fixes

- Scenario builder: indices not subject to random draws could be mixed up in areas
  including both "must-run" units and "regular" units (bug circumscribed to the thermal
  time-series section)

- Spillage management, when numerous binding constraints are active: an excessive leeway
  could be observed regarding the level of hydro power allowed to be curtailed

v4.5.2 (06/2014)
----------------

### Bug fixes

- In the previous version, the average values of interconnection-related variables were multiplied by two
  and this error was propagated to the standard deviation of the same variables

v4.5.1 (06/2014)
----------------

### Features

- Start-up and fixed thermal costs: the contribution of each thermal cluster to the operating
  cost is now explicitly displayed in the results (field : "non proportional cost")


- Load time-series : negative values are now authorized




### Bug fixes

- Creation of a thermal cluster : the default value of the NPOMAX parameter is set to 100


- Hydro energy spatial allocation matrix : values are displayed more clearly in the GUI


- Copy/paste of nodes : the field "spread on unsupplied energy cost" was not pasted


v4.5.0 (04/2014)
----------------

### Features

- Simplex solver: acceleration regarding the control of the admissibility of the solution
  in the dual stages. This brings a significant improvement of the calculation time for
  large problems in which the relative scale of system costs is very wide


- Identical upper and lower bounds have been set for the absolute values of all
  non-zero system costs ( max = 5 10^4 Euros/MWh ; min = 5 10^-3 Euros/MWh)


### Bug fixes

- Hydro Time-series generation : the GUI did not react properly when forbidden
  values (negative) were seized for energy expectation and/or standard deviation


- Unit commitment of thermal plants: the time of the first activation of a plant
  within a week was not fully optimized


v4.4.1 (05/2013)
----------------

### Bug fixes

- Creation of a new binding constraint: the operation needed to be confirmed twice
  (double click on "create button") and the study had to be "saved as" and reloaded before
  proceeding further.

- Time-series analyzer : due to round-off errors, spatial correlation of 100 %
  (perfectly identical sets of time-series in different locations) could sometimes
  be casted to 99%. Exact 100% correlations are now properly displayed.




v4.4.0 (04/2013)
----------------

### Features

- Antares licenses can be either static or floating. Floating tokens are managed and
  distributed by the Flexnet product, version 11.9.

- Thermal plants time-series generator : availability parameters (outage rates and duration)
  corresponding to a Mean Time Between Failure (MTBF) < 1 day are now allowed. Though unusual,
  such sets of parameters may prove useful when it comes to modelling specific situations

- Thermal plants time-series generator : it is possible to model the duration of each kind
  of outages as 365-day random arrays instead of 365-day constant arrays. Two parameters
  are available for the description of the probability distribution function of each component.
  A first parameter allows to set the variable law to either "uniform" or "geometric".
  A second parameter allows to set the ratio of the variable standard deviation to
  its expectation to a particular value

- Thermal plants time-series generator : The planned outage process is now committed to meet a
  set of constraints defined by two 365-day arrays (PO Min Nb, PO Max Nb). For every day of
  each Monte-Carlo year, the actual number of overhauls is kept within the [Min,Max] interval,
  the exact value being determined by regular random draws based on outage rates and durations

- As a consequence of the introduction of these new features, Monte-Carlo time-series
  of available thermal power generated with Antares 4.4 may differ from those generated with
  previous versions. Though differences may be observed draw by draw, the statistical
  properties of the generated time-series are strictly preserved when datasets are identical.

- Hydro storage optimization : when the maximum available power of a given day is not high
  enough to allow the full use of the daily hydro storage energy credit, the energy in excess
  is levelled on the other days of the month with a flatter pattern.


### Bug fixes

- On creation of a new link, the transmission capacity status parameter is set
  to `Use transmission capacities` instead of `Set to null`.



v4.3.7 (02/2013)
----------------

### Features

- Performance improvements for graphical display of large tables


### Bug fixes

- The binding constraint data might not be written properly in some cases
  when the constraint was renamed.



V4.3.6 (12/2012)
----------------

### Bug fixes

- Windows only: fixed potential crash which could happen when exiting
  a simulation in adequacy mode with import of generated time-series

- Windows only: improved free disk space assessment, which now takes into
  consideration user- and folder-related quotas


V4.3.5 (10/2012)
----------------

### Features

- The calendar field "year" is now available in the simulation main screen
  (allows not only simulations from JAN to DEC but also from OCT to SEP, etc.)

- The attribute "Leap year" is now available in the simulation main screen

- The attribute "Week" is now available in the main simulation screen
  (weekly results may be defined not only from MON to SUN but also from SAT to FRI,etc.)

- Time-series screens: a new function is available for hourly and daily time-series
  (shift rows until #date#)

- Linear solver: new version slightly more accurate than the previous one.  
  Note that when a daily or weekly optimization has multiple equally optimal solutions,
  the ultimate choice may differ from that of the previous version


### Bug fixes

- Reference numbers of the time-series used in the course of a simulation:  
  When the simulation is based on a user-defined scenario (building mode: custom)
  and when a printout of the reference numbers of the time-series used in the simulation
  is asked for (MC scenarios: true), the numbers printed for thermal clusters running
  under the "must-run" status were wrong

- Interconnection results, marginal costs:  
  For a congested interconnection whose transmission capacities are not symmetric,
  and in presence of hurdle costs, a zero could sometimes be delivered instead of
  the actually expected value

- Districts: when the Monte-Carlo synthesis edition is skipped, the results regarding
  districts were not accessible via the output viewer.



V4.2.6 (07/2012)
----------------

### Features

- The field "MAX MRG" (last of the nodal results) is now available in the output files

- The Monte-Carlo synthesis edition can be skipped when year-by-year results are asked for


### Bug fixes

- Binding constraints: in the filter available for the weight matrix, removal of
  redundant options

- Copy/Paste nodes on the general map: "print status" parameters can now be copied like
  any other data

- Upgrade of studies in 3.8 format: negative hurdle costs were not correctly transposed

- Thermal plants time-series generator: outages lasting N days, starting on day D, were
  considered as outages lasting N days starting on D+1 (corrected by removal of the
  one-day shift)

- Advanced parameters, option "shave peaks" used along with the "weekly" simplex range:
  the maximum intra-daily hydro storage limit on power could occasionally be overcome during
  the unsupplied energy levelling process (corrected by a slight lessening of the authorized
  levelling)




v4.1.0 (06/2012)
----------------

### Features

- Hydro storage energy management : each nodal policy of use can be tuned so as to
  accommodate simultaneously the net load of  several nodes

- Hydro storage energy modelling : monthly time-series of inflows and reference trajectories
  for reservoir levels can be used instead of monthly time-series of generated energies.

- Load shedding strategies : when unsupplied energy is unavoidable,  a choice is now possible
  between two policies : minimize the duration of sheddings or "shave" the load curve.

- When multiple mathematically equivalent solutions exist a the first order for the
  economic optimization problem, a choice can be made at the second order between three
  ramping strategies




v3.8.0 (12/2011)
----------------

### Features

- The simulation mode `Adequacy` is renamed `Draft`.

- A new simulation mode `Adequacy` is available. In this mode, all thermal plants are
  considered as must-run zero-cost units.

- New possibilities are given regarding the filtering of simulation results (selection
  of nodes, of interconnections, etc.)

- Automatic spatial aggregation of results is possible through the use of the new
  "district" object (a district is a sort of macro-node gathering several regions)

- Nodal costs of unsupplied energy and of spilled energy : a small additive stochastic
  noise around the reference values can be introduced to help discriminate between
  theoretically equivalent solutions



V3.7.4 (08/2011)
----------------

### Features

- New version of the dual simplex engine (speed is about twice that of 3.6 version)

- Economic optimizations now encompass a full week (168 hours) span. Traditional
  day-long optimizations can still be carried out (ad hoc "preference" parameter)

- Binding constraints can be defined at the weekly scale in addition to the
  daily and hourly scales

- Several other "optimization preferences" are made available to allow the quick examination
  of variants used in sensitivity analyses

- A new graphic interface is available for the consultation of all simulation results
  (except those obtained in draft mode)

- Extraction of data regarding any given variable from the whole Monte-Carlo year-by-year
  set of results is now possible

- New variables are introduced in the economic output files : the overall available dispatchable
  thermal generation (AVL DTG) and the thermal margin (DTG MRG = AVL DTG - dispatched power)




V3.6.4 (04/2011)
----------------

### Features

- The "scenario  builder" is now available. With this builder it is possible to  define
  precisely the simulation context (for any given year, random numbers drawn for each
  kind of time-series can be replaced by user-defined numbers). This feature allows
  simulations to be carried out in a versatile "What If" mode.





V3.5.3 (03/2011)
----------------

### Features

- Addition of the fuel category "lignite" to the regular options available
  for the description of thermal plants.

- Improvement of the presentation of the 365-day arrays "market bid modulation"
  and "marginal cost modulation".

- Automatic processing of the inter-monthly & inter-regional hydro correlation hydro
  energy matrix to meet the feasibility constraints (the matrix has to be positive
  semi-definite). User should check in the simulation log file that no warning such as :
  "info : hydro correlation not positive semi-definite : shrink by  factor x " appears.




V3.4.4 (02/2011)
----------------

### Features

- The names of nodes, thermal clusters and binding constraints can be extended to
  128 characters. Authorized characters are : `a-z, A-Z,0-9,-,_, space`




v3.4.3 (10/2010)
----------------

### Features

- Two calculations modes are now available (in the "run" window):

  "regular": the software tries to hold all simulation data in RAM
  this mode is faster than the second one when datasets are small but
  can get dramatically slow when RAM limits are close

  "swap" : a dedicated memory management module loads in RAM amounts
  of data as small as possible. This mode should be prefered to the
  other when datasets are large.

  Note that in "regular" mode, the maximum amount of data loaded is
  limited by the OS to 2 Go on 32-bit machines, regardless of the
  memory installed. The integrality of installed memory can be used
  on 64-bit machines.

- A new module (time-series analyzer) is available to help set the
  parameters of the stochastic time-series generators for wind power,
  solar power and load. The analyzer determines, on sets of historical
  8760-hour time-series the relevant parameters for different kinds of
  random laws (uniform, normal,Weibull, Beta, Gamma), along with a
  description of the auto-correlation dynamic (two parameters)
  and a full spatial correlation matrix





v3.3.2 (07/2010)
----------------

### Features

- Improvement of the wind power time-series generator (faster calculations)

- Introduction of new stochastic time-series generators for
  solar power and load

- Introduction of an explicit  modelling of wind-to-power curves.
  As a consequence, wind power time-series can now be generated
  either through a direct approach (by analysis of historical
  time-series of power) or through an indirect (more physical)
  approach, based on the analysis of historical time-series of
  wind speed

- Introduction of a new 8760-hour power array for each node,
  representing the day-ahead reserve that should be made available
  (either on-site or at distance) to face last-minute incidents
  and/or forecasts errors.

- Introduction of so-called hurdles costs on interconnection.




v3.1.0 (01/2010)
----------------

### Features

- Breakdown of monthly hydro storage energy credits in daily credits:
  The pilot curve is now the net load (i.e. load - all must-run generation)
  instead of the gross load

- New functionalities available for datasets management (stucy cleaner,
  Log file wiewer)

- New info is given for simulation context (available & required amounts
  of RAM & HDD space)



From V1 to V2 (all versions)
----------------------------

- Refer to project development archives (TRAC thread)

