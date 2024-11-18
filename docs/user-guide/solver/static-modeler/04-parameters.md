# Parameters

[//]: # (TODO: complete this page)
_**This section is under construction**_  

An *Antares* study contains a global parameter file named `generaldata.ini`.

---
## General parameters
These parameters are listed under the `[general]` section in the `.ini` file.

---
### Study mode


#### mode
[//]: # (TODO: verify if required, remove default value)
[//]: # (TODO: add details 'expansion' behavior)
- **Expected value:** one of the following (case-insensitive):
    - `economy` or `economic`
    - `adequacy`
    - `expansion`
- **Required:** **yes**
- **Default value:** `economy`
- **Usage:** this parameter sets the study mode for Antares
    - `economy/economic`: Antares simulator will try to ensure balance between load and generation, while minimizing the 
      economical cost of the grid's operation (more on this [here](../../01-overview.md#transmission-project-profitability)). "Economy" simulations make a full use of 
      *Antares* optimization capabilities. They require economic as well as technical input data and may demand a lot 
      of computer resources.
    - `adequacy`: in this mode, all power plants' operational cost is considered zero. Antares' only objective is to ensure
      balance between load and generation (more on this [here](../../01-overview.md#generation-adequacy-problems)). "Adequacy" simulations are faster and require only 
      technical input data. Their results are limited to adequacy indicators.
    - `expansion`: Antares simulator will optimize the investments on the grid, minimizing both investments and
      operational costs.

---
### Study horizon


#### horizon
[//]: # (TODO: verify if required, remove default value)
- **Expected value:** year (string)
- **Required:** **yes**
- **Default value:** empty string
- **Usage:** the horizon of the study (static tag, not used in the calculations)

---

### Calendar parameters


#### nbyears
[//]: # (TODO: verify if required, verify default value)
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Default value:**
- **Usage:** number of Monte-Carlo (MC) years that should be prepared for the simulation (not always the same as the
  MC years actually simulated, which are defined by [user-playlist](#user-playlist) and [playlist parameters](#playlist-parameters)).

---
#### simulation.start
[//]: # (TODO: verify if required, verify default value)
[//]: # (TODO: verify if index 8 should be replaced by 7 in example -starts at 1 or at 0 ?-)
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Default value:** 0
- **Usage:** index of first day to include in study (>= 0) (e.g. 8 for a simulation beginning on the 
  second week of the first month of the year)

---
#### simulation.end
[//]: # (TODO: verify if required, verify default value)
[//]: # (TODO: verify if index starts at 1 or at 0 for example)
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Default value:** 365
- **Usage:** index of last day to include in study (<= 365) (e.g. 28 for a simulation ending on the fourth week 
  of the first month of the year)
  
> _**Note:**_ 
> In Economy an Adequacy [simulation modes](#mode), simulation start & end must be chosen to make the simulation span a 
> round number of weeks. If not, the simulation span will be truncated: for instance, (1, 365) will be interpreted as 
> (1, 364), i.e. 52 weeks (the last day of the last month will not be simulated).

---
#### january.1st
[//]: # (TODO: verify default value)
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`,
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`,
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday`
- **Usage:** this parameter should indicate the day-of-week of january 1st of the given [year](#horizon).

---
#### first-month-in-year
[//]: # (TODO: verify if required, remove default value)
- **Expected value:** one of the following string (case-insensitive): `jan`/`january`, `feb`/`february`, `mar`/`march`,
  `apr`/`april`, `may`, `jun`/`june`, `jul`/`july`, `aug`/`august`, `sep`/`september`, `oct`/`october`,
  `nov`/`november`, `dec`/`december`.
- **Required:** **yes**
- **Default value:**
- **Usage:** this is the first month in the input time-series, of the studied [year](#horizon).

---
#### first.weekday
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`,
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`,
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday`
- **Usage:** in economy or adequacy simulations, indicates the frame (Mon-Sun, Sat-Fri, etc.) to use for the edition 
  of weekly results.

---
#### leapyear
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** indicates if the studied [year](#horizon) is a leap-year.

---
### Additional parameters


#### year-by-year
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:**
  - `false`: no individual results will be printed out
  - `true`: for each simulated year, detailed results will be printed out in an individual directory:
    `Study_name/OUTPUT/simu_tag/Economy/mc-i-number

---
#### derated
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** if set to `true`, all time-series will be replaced by their general average and the number of MC years
  set to 1. If the TS are ready-made or Antares-generated but are not to be stored in the INPUT folder,
  no time-series will be written over the original ones (if any). If the time-series are built by Antares
  and if it is specified that they should be stored in the INPUT, a single average-out time series will be stored
  instead of the whole set.

> _**WARNING:**_ this parameter cannot be used with parameter [custom-scenario](#custom-scenario)

---
#### custom-scenario
[//]: # (TODO: add specific link to "scenario builder")
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** 
  - `false`: for all years to simulate, all time-series will be drawn at random
  - `true`: the simulation will be carried out on a mix of deterministic and probabilistic conditions,
    with some time-series randomly drawn and others set to user-defined values. This option allows setting
    up detailed "what if" simulations that may help to understand the phenomena at work and quantify various kinds
    of risk indicators. To set up the simulation profile, use the [scenario builder](02-inputs.md).

> _**WARNING:**_ this parameter cannot be used with parameter [derated](#derated)  
> _**WARNING:**_ if set to `true`, parameter [active-rules-scenario](#active-rules-scenario) must be set

---
#### user-playlist
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:**
  - `false`: all prepared Monte-Carlo (MC) years will actually be simulated.
  - `true`: the years to simulate are defined in a list. To set up this list, use the [playlist parameters](#playlist-parameters).  
    This feature allows, for instance, to refine a previous simulation by excluding a small number of "raw" MC years
    whose detailed analysis may have shown that they were not physically realistic. A different typical use case consists
    in replaying only a small number of years of specific interest (for instance, years in the course of which Min or Max
    values of a given variable were encountered in a previous simulation).  
    In addition, each MC year i=1, …, N can be given a relative [weight](#playlist_year_weight)

> _**WARNING:**_ this parameter cannot be used with parameter [derated](#derated)

---
#### thematic-trimming
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` in order to select a specific subset of the optimization variables to print in the [output files](03-outputs.md),
  using a thematic filter. Use the [variables selection parameters](#variables-selection-parameters) to define the filter.  
  Thematic Trimming does not reduce computation time, but can bring some benefits on total runtime (smaller files to 
  write). It can save a lot of disk space in simulations where only a few variables are of interest.

---
#### geographic-trimming
[//]: # (TODO: verify usage)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:**    
  Allows to select the results to store at the end of a simulation: choice of areas, interconnections, temporal 
  aggregations (hourly, daily, etc.).  
  Geographic Trimming does not reduce computation time, but can bring some benefits on total runtime (fewer files to 
  write). It can save a lot of disk space in simulations where only a few Areas and Links are of interest.
  - **None** Storage of results for all areas, geographic districts, interconnections as well as all time spans
    (hourly, daily, etc.)
  - **Custom** Storage of the results selected with the "Geographic Trimming" command of the "Configure"
    option available in the main menu.
    Filters on areas, interconnections and time spans may also be defined as follows:
    - On the map, select area(s) and/or interconnection(s)
    - Open the inspector module (Main menu, Windows)
    - Set adequate parameters in the "output print status" group

---
#### active-rules-scenario
[//]: # (TODO: add usage details)
- **Expected value:** undefined, or `default ruleset` (case-insensitive)
- **Required:** no, unless [custom-scenario](#custom-scenario) is set to `true`
- **Default value:** undefined
- **Usage:** defines how scenarios are built. Only one supported value for now:
    - `default ruleset`

---
### Pre-processor parameters


#### readonly
[//]: # (TODO: add usage details)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** sets the study to read-only mode

---
## Output parameters
These parameters are listed under the `[output]` section in the `.ini` file.

---
#### synthesis
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:**
    - `true`: synthetic results will be stored in a directory: `Study_name/OUTPUT/simu_tag/Economy/mc-all`
    - `false`: no general synthesis will be printed out

---
#### storenewset
[//]: # (TODO: verify usage)
[//]: # (TODO: link to specific output folder when it is documented in Outputs doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:**
    - `false`: no storage of the time-series numbers (either randomly drawn or user-defined) used to set up the simulation.
    - `true`: a [specific output folder](03-outputs.md) will be created to store all the time-series numbers drawn 
      when preparing the Monte-Carlo years.

---
#### hydro-debug
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
#### result-format
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
#### archives
[//]: # (TODO: fill default value)
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:**
- **Usage:** if [storenewset](#storenewset) is set to `true`, this parameter selects those of them that should be
  written in the [output files](03-outputs.md).

---
## Optimization parameters
[//]: # (TODO: add link to "local parameter values" documentation)
Options related to the optimization core used in the simulations.
This set of parameters is study-specific; it can be changed at any time and saved along with study data.
The values set in this file take precedence over the local parameter values. For instance, if the LOCAL
parameter "set to infinite" is activated for some interconnections, and if the GLOBAL preference regarding transmission
capacities is "set to null", the simulation will be carried out as if there were no longer any grid BUT the local
values will remain untouched. If the preference is afterward set to "local values", the interconnections will be
given back their regular capacities (infinite for those being set on "set to infinite").  
These parameters are listed under the `[optimization]` section in the `.ini` file.

---
#### simplex-range
- **Expected value:** `day` or `week`
- **Required:** no
- **Default value:** `week`
- **Usage:** the simplex optimization range.  
  In the formulation of the optimal hydro-thermal unit-commitment and dispatch problem (see dedicated document),
  the reference hydro energy $HIT$ used to set the right hand sides of hydro- constraints depends on the value
  chosen for this parameter, and is defined as follows:
    - `day`: for each day **d** of week $\omega$ : $HIT = W_d^2$
    - `week`: for week $\omega$: $HIT = \sum_{d\in \omega}{W_d^2}$  
  Weekly optimization performs a more refined unit commitment, especially when [unit-commitment-mode](#unit-commitment-mode) 
  is set to `accurate`.

> More information here: [The heuristic for seasonal hydro pre-allocation](06-hydro-heuristics.md#seasonal-hydro-pre-allocation)

---
#### transmission-capacities
- **Expected value:** one of the following (case-insensitive):
    - `local-values`
    - `null-for-all-links`
    - `infinite-for-all-links`
    - `infinite-for-physical-links`
    - `null-for-physical-links`
- **Required:** no
- **Default value:** `local-values`
- **Usage:** allows the user to override the transmission capacities on links.
    - `local-values`: use the local property for all links, including physical links (no override)
    - `null-for-all-links`: override all transmission capacities with 0
    - `infinite-for-all-links`: override all transmission capacities with $\inf$
    - `infinite-for-physical-links`: override transmission capacities with $\inf$ on **physical links only**
    - `null-for-physical-links`: override transmission capacities with 0 on **physical links only**

---
#### link-type
[//]: # (TODO: document this parameter, seems to belong to another category)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

> _**WARNING:**_ this parameter is ignored since version 8.5.2

---
#### include-constraints
[//]: # (TODO: add link to binding constraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate binding constraints in the study. 

---
#### include-hurdlecosts
[//]: # (TODO: document usage)
_**This section is under construction**_  

- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:**

---
#### include-loopflowfee
[//]: # (TODO: document this parameter, seems to be deprecated)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
#### include-tc-minstablepower
[//]: # (TODO: add link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate, for thermal units, the constraint of minimum stable power

---
#### include-tc-min-ud-time
[//]: # (TODO: add link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate, for thermal units, the constraint of minimum start-up time

---
#### include-dayahead
[//]: # (TODO: add details + link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate day-ahead reserve constraints

---
#### include-strategicreserve
[//]: # (TODO: add details + link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate strategic reserve constraints

---
#### include-spinningreserve
[//]: # (TODO: add details + link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate spinning reserve constraints

---
#### include-primaryreserve
[//]: # (TODO: add details + link to contraints doc)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate primary reserve constraints

---
#### include-exportmps
[//]: # (TODO: in usage paragraph, add links to relevant doc of "first step" and "second step" of optimization)
- **Expected value:** one of the following (case-insensitive):
    - `none` or `false`
    - `optim-1`
    - `optim-2`
    - `both-optims` or `true`
- **Required:** no
- **Default value:** `none/false`
- **Usage:** use this parameter if you want to export the optimization problems in [MPS format](https://en.wikipedia.org/wiki/MPS_(format)):  
    - `none` or `false`: do not export any MPS
    - `optim-1`: export MPS for firt step of the optimization
    - `optim-2`: export MPS for second step of the optimization
    - `both-optims` or `true`: export MPS for both steps of the optimization

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-include-exportmps-parameter).

---
#### include-split-exported-mps
[//]: # (TODO: document this parameter, seems to belong to another category)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

> _**WARNING:**_ this parameter is deprecated but is still needed for testing old studies

---
#### include-exportstructure
[//]: # (TODO: add link to AntaresXpansion)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` to activate writing the [MPS](#include-exportmps) using the *AntaresXpansion* specific format

---
#### include-unfeasible-problem-behavior
- **Expected value:** one of the following (case-sensitive):
    - `WARNING_DRY`
    - `WARNING_MPS`
    - `ERROR_DRY`
    - `ERROR_MPS`
- **Required:** no
- **Default value:** `ERROR_MPS`
- **Usage:** defines the behavior of the simulator in case of an unfeasible problem.
    - `WARNING_DRY`: continue simulation
    - `WARNING_MPS`: continue simulation, but export the MPS of the unfeasible problem
    - `ERROR_DRY`: stop simulation
    - `ERROR_MPS`: stop simulation, and export the MPS of the unfeasible problem  
  
> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-include-unfeasible-problem-behavior-parameter).

---
#### solver-parameters
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
## Adequacy-patch parameters
Defines a set of options related to the [adequacy patch](../optional-features/adequacy-patch.md).
The set of preferences is study-specific; it can be changed at any time and saved along with study data.  
These parameters are listed under the `[adequacy patch]` section in the `.ini` file.  

---
#### include-adq-patch
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set this parameter to `true` if you want to enable the [Adequacy Patch](../optional-features/adequacy-patch.md) algorithm.

---
#### set-to-null-ntc-from-physical-out-to-physical-in-for-first-step
[//]: # (TODO: usage is not clear)
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** Transmission capacities from physical areas outside adequacy patch (area type 1) to physical areas
  inside adequacy patch (area type 2). NTC is set to null (if true) only in the first step of adequacy patch local matching rule.  
  NTC from physical areas outside to physical areas inside adequacy patch (set to null / local values)

---
#### price-taking-order
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:** Price taking order (DENS / Load)

---
#### include-hurdle-cost-csr
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:** Include hurdle cost in CSR optimization (false / true)

---
#### check-csr-cost-function
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:** Check CSR cost function value prior and after CSR (false / true)

---
#### threshold-initiate-curtailment-sharing-rule
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
#### threshold-display-local-matching-rule-violations
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
#### threshold-csr-variable-bounds-relaxation
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

---
## Other parameters
These parameters are listed under the `[other preferences]` section in the `.ini` file.

---
#### initial-reservoir-levels (DEPRECATED since 9.2: cold start is default behavior)
- **Expected value:** one of the following (case-insensitive):
    - `cold start`
    - `hot start`
- **Required:** no
- **Default value:** `cold start`
- **Usage:** initial reservoir levels:
    - `cold start`
    - `hot start`

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-initial-reservoir-levels-parameter).

---
#### hydro-heuristic-policy
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** one of the following (case-insensitive):
    - `accommodate rule curves`
    - `maximize generation`
- **Required:** no
- **Default value:** `accommodate rule curves`
- **Usage:** hydraulic heuristic policy:
    - `accommodate rule curves`
    - `maximize generation`

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-hydro-heuristic-policy-parameter).

---
#### hydro-pricing-mode
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** one of the following (case-insensitive):
    - `fast`
    - `accurate`
- **Required:** no
- **Default value:** `fast`
- **Usage:**
    - `fast`
    - `accurate`: Note that this mode is significantly slower than the `fast` mode.

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-hydro-pricing-mode-parameter).

---
#### power-fluctuations
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** one of the following (case-insensitive):
    - `minimize ramping`
    - `free modulations`
    - `minimize excursions`
- **Required:** no
- **Default value:** `free modulations`
- **Usage:**
    - `minimize ramping`
    - `free modulations`
    - `minimize excursions`

---
#### shedding-policy
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** one of the following (case-insensitive):
    - `shave peaks`
    - `minimize duration`
- **Required:** no
- **Default value:** `shave peaks`
- **Usage:** power shedding policy:
    - `shave peaks`:
    - `minimize duration`:

---
#### unit-commitment-mode
[//]: # (TODO: complete the usage paragraph by adding details & links to relevant UC doc)
- **Expected value:** one of the following (case-insensitive):
    - `fast`
    - `accurate`
    - `milp`
- **Required:** no
- **Default value:** `fast`
- **Usage:** choose the mode in which Antares resolves the unit-commitment problem:
    - `fast`: Heuristic in which 2 LP problems are solved. No explicit modelling for the number of ON/OFF units.
    - `accurate`: Heuristic in which 2 LP problems are solved. Explicit modelling for the number of ON/OFF units. Slower than `fast`.
    - `milp`: A single MILP problem is solved, with explicit modelling for the number of ON/OFF units. Slower than `accurate`.

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-unit-commitment-mode-parameter).

---
#### number-of-cores-mode
- **Expected value:** one of the following (case-insensitive):
    - `minimum`
    - `low`
    - `medium`
    - `high`
    - `maximum`
- **Required:** no
- **Default value:** `medium`
- **Usage:** use this parameter to configure [multi-threading](../optional-features/multi-threading.md).

---
#### renewable-generation-modelling
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** one of the following (case-insensitive):
    - `aggregated`
    - `clusters`
- **Required:** no
- **Default value:** `aggregated`
- **Usage:**
    - `aggregated`
    - `clusters`

> _**Note:**_ You can find more information on this parameter [here](../03-appendix.md#details-on-the-renewable-generation-modelling-parameter).

---
#### day-ahead-reserve-management
[//]: # (TODO: document this parameter, doesn't seem to belong to this category)
- **Expected value:** 
- **Required:** **yes** 
- **Default value:** 
- **Usage:** 

> _**WARNING:**_ this parameter is ignored since version 8.4

---
## Advanced parameters
Advanced Parameters allow to adjust the simulation behavior regarding issues
that are more numerical than physical. The set of parameters is study-specific and can be updated at any time.  
These parameters are listed under the `[advanced parameters]` section in the `.ini` file.

---
#### accuracy-on-correlation
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:**

---
#### adequacy-block-size
[//]: # (TODO: document this parameter, seems to belong to another category)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**

> _**WARNING:**_ this parameter is ignored since version 8.5

---
## Seeds - Mersenne Twister parameters
These parameters are listed under the `[seeds - Mersenne Twister]` section in the `.ini` file.  
They allow the user to set the seeds of random number generators, in order to ensure the results are repeatable.

---
#### seed-tsnumbers
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random [Monte-Carlo years](#nbyears) selection.

---
#### seed-unsupplied-energy-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on unsupplied energy costs.

---
#### seed-spilled-energy-costs
[//]: # (TODO: verify if required, if not, add default value)
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on spilled energy costs.

---
#### seed-thermal-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on thermal plants' costs.

---
#### seed-hydro-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on hydraulic plants' costs.

---
#### seed-initial-reservoir-levels
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:**

---
## Playlist parameters
These parameters are listed under the `[playlist]` section in the `.ini` file.  
They are **required** if [user-playlist](#user-playlist) is set to `true`.

---
#### playlist_reset
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** `true` or `false`
- **Required:** **yes**, if [user-playlist](#user-playlist) is set to `true`.
- **Usage:**

---
#### playlist_year
- **Expected value:** `+ =` or `- =`, followed by a positive integer (example: `playlist_year + = 5`)
- **Required:** **yes**, if [user-playlist](#user-playlist) is set to `true`, ignored otherwise.
- **Usage:**
    - for every Monte-Carlo year that you want the Antares Simulator to **study**, add the parameter entry
      `playlist_year + = i`, where `i` is the index of the year.
    - for every Monte-Carlo year that you want the Antares Simulator to **skip**, add the parameter entry
      `playlist_year - = i`, where `i` is the index of the year.

_Please note that by convention, the first year has index 0._

---
#### playlist_year_weight
[//]: # (TODO: document this parameter)
_**This section is under construction**_  

- **Expected value:**
- **Required:** **yes**
- **Default value:**
- **Usage:**
Each MC year i=1, …, N can be given a relative “weight” $W_i$  in the simulation (default value: 1). 
The expectation and standard deviation of all random variables will then be computed as if the scenarios simulated were 
sampled from a probability density function in which MC year i is given the probability 
$$\frac{W_{i}}{\sum_{j=1,...,N}{W_{j}}}$$

---
## Variables selection parameters
These parameters are listed under the `[variables selection]` section in the `.ini` file.  
They are **required** if [thematic-trimming](#thematic-trimming) is set to `true`.

---
#### selected_vars_reset
[//]: # (TODO: complete the usage paragraph)
- **Expected value:** `true` or `false`
- **Required:** **yes**, if [thematic-trimming](#thematic-trimming) is set to `true`.
- **Usage:**

---
#### select_var
- **Expected value:** `+ =` or `- =`, followed by a string (example: `select_var + = LOAD`)
- **Required:** **yes**, if [thematic-trimming](#thematic-trimming) is set to `true`.
- **Usage:**
    - for every optimization variable that you want the Antares Simulator to **export** in the outputs, add the
      parameter entry `select_var + = VAR`, where `VAR` is the name of the variable.
    - for every optimization variable that you want the Antares Simulator to **omit** from the outputs, add the
      parameter entry `select_var - = VAR`, where `VAR` is the name of the variable.

> _**WARNING:**_ this parameter is intended for use by expert users that know the name of the optimization variables created
> under-the-hood by Antares Simulator.

---
## Model-wise parameters
[//]: # (TODO: link to model-wise parameters documentation)
_**This section is under construction**_ 