## General parameters
These parameters are listed under the `[general]` section in the `.ini` file.  

### Study mode

#### mode
- **Expected value:** one of the following (case-insensitive):
    - `economy` or `economic`
    - `adequacy`
    - `expansion`
- **Required:** **yes** TODO
- **Default value:** `economy`
- **Usage:** this parameter sets the study mode for Antares
    - `economy/economic`: Antares simulator will try to ensure balance between load and generation, while minimizing the economical 
    cost of the grid's operation (more on this [here](01-introduction.md#transmission-project-profitability)).
    - `adequacy`: in this mode, all power plants' operational cost is considered zero. Antares' only objective is to ensure 
    balance between load and generation (more on this [here](01-introduction.md#generation-adequacy-problems)). 
    - `expansion`: Antares simulator will optimize the investments on the grid, minimizing both investments and 
    operational costs. If selected, the simulator will TODO

### Study horizon

#### horizon
- **Expected value:** year (string)
- **Required:** **yes** TODO
- **Default value:** empty string
- **Usage:** the horizon of the study

### Calendar parameters

#### nbyears
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** number of Monte-Carlo years to simulate

#### simulation.start
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** 0 ? TODO
- **Usage:** index of first day to include in study (>= 0)

#### simulation.end
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** 365 ? TODO
- **Usage:** index of last day to include in study (<= 365)

#### january.1st
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`, 
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`, 
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday` TODO
- **Usage:** this parameter should indicate the day-of-week of january 1st of the given [year](#horizon).

#### first-month-in-year
- **Expected value:** one of the following string (case-insensitive): `jan`/`january`, `feb`/`february`, `mar`/`march`, 
  `apr`/`april`, `may`, `jun`/`june`, `jul`/`july`, `aug`/`august`, `sep`/`september`, `oct`/`october`, 
  `nov`/`november`, `dec`/`december`.
- **Required:** **yes**
- **Default value:** TODO
- **Usage:** this is the first month in the studied [year](#horizon).

#### first.weekday
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`,
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`,
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday`
- **Usage:** this is the day-of-week of the first day in the studied [horizon](#horizon).

#### leapyear
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** indicates if the studied [year](#horizon) is a leap-year.

### Additional parameters

#### year-by-year
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** 
    - `true`: results will be exported on a yearly basis
    - `false`: results will be all in one file TODO

#### derated
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO  

> ⚠️ this parameter cannot be used with parameter [custom-scenario](#custom-scenario)

#### custom-scenario
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO  

> ⚠️ this parameter cannot be used with parameter [derated](#derated)  
> ⚠️ if set to `true`, parameter [active-rules-scenario](#active-rules-scenario) must be set

#### user-playlist
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` in order to select specific Monte-Carlo years to simulate. 
  Use the [playlist parameters](#playlist-parameters) to select these years.  

> ⚠️ this parameter cannot be used with parameter [derated](#derated)

#### thematic-trimming
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` in order to select a specific subset of the optimization variables to print in the [output files](05-output_files.md), 
  using a thematic filter. Use the [variables selection parameters](#variables-selection-parametrs) to define the filter.

#### geographic-trimming
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO

#### active-rules-scenario
- **Expected value:** undefined, or `default ruleset` (case-insensitive)
- **Required:** no, unless [custom-scenario](#custom-scenario) is set to `true`
- **Default value:** undefined
- **Usage:** TODO defines how scenarios are built. Only one supported value for now:
    - `default ruleset`: TODO

### Time-series parameters

#### generate
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive): 
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `max-power` (ex: `generate = load, hydro, wind`)
- **Required:** no
- **Default value:** empty
- **Usage:** if left empty, all the time-series will be imported from input files. Else, the time-series 
  listed in this parameter will be generated randomly by the simulator.  

> ⚠️ time-series generation is not available for transmission capacities (NTC)

#### nbtimeseriesload
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `load` time-series are [automatically generated](#generate), this parameter sets the number of different 
  time-series to generate.

#### nbtimeserieshydro
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `hydro` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

#### nbtimeserieswind
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `wind` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

#### nbtimeseriesthermal
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `thermal` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

#### nbtimeseriessolar
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `solar` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

#### refreshtimeseries
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that have 
  to be refreshed on-line ?? TODO  

> ⚠️ time-series refresh is not available for transmission capacities (NTC)

#### intra-modal
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that are 
  correlated, i.e. for which the same time-series should be used together in a given scenario. ?? TODO  

> ⚠️ inter-modal correlation is not available for transmission capacities (NTC)  
> ⚠️ this is the historical correlation mode

#### inter-modal
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

#### refreshintervalload
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `load` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this 
  parameter sets their refresh interval (in number of Monte-Carlo years).

#### refreshintervalhydro
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `hydro` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

#### refreshintervalwind
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `wind` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

#### refreshintervalthermal
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `thermal` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

#### refreshintervalsolar
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `solar` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

### Pre-processor parameters

#### readonly
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** sets the study to read-only mode: TODO

## Input parameters
These parameters are listed under the `[input]` section in the `.ini` file.

#### import
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that 
  should be exported into the input files.

## Output parameters
These parameters are listed under the `[output]` section in the `.ini` file.

#### synthesis
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set to `true` if you want to write the Monte-Carlo simulation synthesis int the [output files](05-output_files.md).

#### storenewset
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` to store the sampled timeseries numbers in the [output files](05-output_files.md).

> Related to: [Time-series parameters](#time-series-parameters) 

#### archives
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** TODO
- **Usage:** if [storenewset](#storenewset) is set to `true`, this parameter selects those of them that should be 
  written in the [output files](05-output_files.md).

## Optimization parameters
These parameters are listed under the `[optimization]` section in the `.ini` file.

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
  
> More information here: [The heuristic for seasonal hydro pre-allocation](08-miscellaneous.md#the-heuristic-for-seasonal-hydro-pre-allocation)

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

#### link-type
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO
  
> ⚠️ this parameter is ignored since version 8.5.2

#### include-constraints
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate binding constraints in the study. TODO add link to binding constraints doc

#### include-hurdlecosts
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** TODO

#### include-tc-minstablepower
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate, for thermal units, the constraint of minimum stable power
  TODO add link to doc

#### include-tc-min-ud-time
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate, for thermal units, the constraint of minimum start-up time
  TODO add link to doc

#### include-dayahead
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate day-ahead reserve constraints ??
  TODO add link to doc

#### include-strategicreserve
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate strategic reserve constraints ??
  TODO add link to doc

#### include-spinningreserve
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate spinning reserve constraints ??
  TODO add link to doc

#### include-primaryreserve
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** set this parameter to `true` if you want to activate primary reserve constraints ??
  TODO add link to doc

#### include-exportmps
- **Expected value:** one of the following (case-insensitive):
    - `none` or `false`
    - `optim-1`
    - `optim-2`
    - `both-optims` or `true`
- **Required:** no
- **Default value:** `none/false`
- **Usage:** use this parameter if you want to export the optimization problems in [MPS format](https://en.wikipedia.org/wiki/MPS_(format)):  
  TODO specify where the files are written 
    - `none` or `false`: do not export any MPS
    - `optim-1`: export MPS for [firt step]() of the optimization TODO add link
    - `optim-2`: export MPS for [second step]() of the optimization TODO add link
    - `both-optims` or `true`: export MPS for both steps of the optimization

#### include-split-exported-mps
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO
> ⚠️ this parameter is deprecated but needed for testing old studies

#### include-exportstructure
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` to activate writing the [MPS](#include-exportmps) using the [AntaresXpansion](TODO add link) specific format

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


## Adequacy-patch parameters
These parameters are listed under the `[adequacy patch]` section in the `.ini` file. They tune the [Adequacy Patch](14-adequacy-patch.md) 
algorithm.

#### include-adq-patch
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set this parameter to `true` if you want to enable the [Adequacy Patch](14-adequacy-patch.md) algorithm.

#### set-to-null-ntc-from-physical-out-to-physical-in-for-first-step
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** TODO Transmission capacities from physical areas outside adequacy patch (area type 1) to physical areas 
  inside adequacy patch (area type 2). NTC is set to null (if true) only in the first step of adequacy patch local matching rule.

#### set-to-null-ntc-between-physical-out-for-first-step
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `true`
- **Usage:** TODO Transmission capacities between physical areas outside adequacy patch (area type 1).
  NTC is set to null (if true) only in the first step of adequacy patch local matching rule.

## Other parameters
These parameters are listed under the `[other preferences]` section in the `.ini` file.

#### initial-reservoir-levels
- **Expected value:** one of the following (case-insensitive):
    - `cold start`
    - `hot start`
- **Required:** no
- **Default value:** `cold start`
- **Usage:** initial reservoir levels:
    - `cold start`: TODO
    - `hot start`: TODO

#### hydro-heuristic-policy
- **Expected value:** one of the following (case-insensitive):
    - `accommodate rule curves`
    - `maximize generation`
- **Required:** no
- **Default value:** `accommodate rule curves`
- **Usage:** hydraulic heuristic policy:
    - `accommodate rule curves`: TODO
    - `maximize generation`: TODO

#### hydro-pricing-mode
- **Expected value:** one of the following (case-insensitive):
    - `fast`
    - `accurate`
- **Required:** no
- **Default value:** `fast`
- **Usage:** TODO
    - `fast`: TODO
    - `accurate`: TODO. Please take into consideration that this mode is significantly slower than the fast one.

#### power-fluctuations
- **Expected value:** one of the following (case-insensitive):
    - `minimize ramping`
    - `free modulations`
    - `minimize excursions`
- **Required:** no
- **Default value:** `free modulations`
- **Usage:** TODO:
    - `minimize ramping`: TODO
    - `free modulations`: TODO
    - `minimize excursions`: TODO

#### shedding-policy
- **Expected value:** one of the following (case-insensitive):
    - `shave peaks`
    - `minimize duration`
- **Required:** no
- **Default value:** `shave peaks`
- **Usage:** TODO power shedding policy:
    - `shave peaks`: TODO
    - `minimize duration`: TODO

#### unit-commitment-mode
- **Expected value:** one of the following (case-insensitive):
    - `fast`
    - `accurate`
    - `milp`
- **Required:** no
- **Default value:** `fast`
- **Usage:** choose the mode in which Antares resolves the unit-commitment problem:
    - `fast`: Heuristic in which 2 LP problems are solved. No explicit modelling for the number of ON/OFF units. TODO: add details/links
    - `accurate`: Heuristic in which 2 LP problems are solved. Explicit modelling for the number of ON/OFF units. Slower than `fast`. TODO: add details/links
    - `milp`: A single MILP problem is solved, with explicit modelling for the number of ON/OFF units. Slower than `accurate`. TODO: add details/links

#### number-of-cores-mode
- **Expected value:** one of the following (case-insensitive):
    - `minimum`
    - `low`
    - `medium`
    - `high`
    - `maximum`
- **Required:** no
- **Default value:** `medium`
- **Usage:** TODO:
    - `minimum`: TODO
    - `low`: TODO
    - `medium`: TODO
    - `high`: TODO
    - `maximum`: TODO

#### renewable-generation-modelling
- **Expected value:** one of the following (case-insensitive):
    - `aggregated`
    - `clusters`
- **Required:** no
- **Default value:** `aggregated`
- **Usage:** TODO:
    - `aggregated`: TODO
    - `clusters`: TODO

#### day-ahead-reserve-management
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO
  
> ⚠️ this parameter is ignored since version 8.4


## Advanced parameters
These parameters are listed under the `[advanced parameters]` section in the `.ini` file.

#### accuracy-on-correlation
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** TODO

#### adequacy-block-size
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

> ⚠️ this parameter is ignored since version 8.5

## Seeds - Mersenne Twister parameters
These parameters are listed under the `[seeds - Mersenne Twister]` section in the `.ini` file.  
They allow the user to set the seeds of random number generators, in order to ensure the results are repeatable.

#### seed-tsgen-load
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `load` time-series are [automatically generated](#generate), this parameter fixes the seed for its 
  random generator.

#### seed-tsgen-hydro
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `hydro` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

#### seed-tsgen-wind
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `wind` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

#### seed-tsgen-thermal
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `thermal` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

#### seed-tsgen-solar
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `solar` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

#### seed-tsnumbers
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random [Monte-Carlo years](#nbyears) selection.

#### seed-unsupplied-energy-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on unsupplied energy costs.

#### seed-spilled-energy-costs
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Usage:** fixes the seed for the random noise generation on spilled energy costs.

#### seed-thermal-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on thermal plants' costs.

#### seed-hydro-costs
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** fixes the seed for the random noise generation on hydraulic plants' costs.

#### seed-initial-reservoir-levels
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** TODO


## Playlist parameters
These parameters are listed under the `[playlist]` section in the `.ini` file.  
They are **required** if [user-playlist](#user-playlist) is set to `true`.  

#### playlist_reset
- **Expected value:** `true` or `false`
- **Required:** **yes**, if [user-playlist](#user-playlist) is set to `true`.
- **Usage:** TODO

#### playlist_year
- **Expected value:** `+ =` or `- =`, followed by a positive integer (example: `playlist_year + = 5`) 
- **Required:** **yes**, if [user-playlist](#user-playlist) is set to `true`.
- **Usage:**
    - for every Monte-Carlo year that you want the Antares Simulator to **study**, add the parameter entry 
      `playlist_year + = i`, where `i` is the index of the year.
    - for every Monte-Carlo year that you want the Antares Simulator to **skip**, add the parameter entry 
      `playlist_year - = i`, where `i` is the index of the year.


## Variables selection parameters
These parameters are listed under the `[variables selection]` section in the `.ini` file.  
They are **required** if [thematic-trimming](#thematic-trimming) is set to `true`.

#### selected_vars_reset
- **Expected value:** `true` or `false`
- **Required:** **yes**, if [thematic-trimming](#thematic-trimming) is set to `true`.
- **Usage:** TODO

#### select_var
- **Expected value:** `+ =` or `- =`, followed by a string (example: `select_var + = LOAD`)
- **Required:** **yes**, if [thematic-trimming](#thematic-trimming) is set to `true`.
- **Usage:**
    - for every optimization variable that you want the Antares Simulator to **export** in the outputs, add the
      parameter entry `select_var + = VAR`, where `VAR` is the name of the variable.
    - for every optimization variable that you want the Antares Simulator to **omit** from the outputs, add the 
      parameter entry `select_var - = VAR`, where `VAR` is the name of the variable.

> ⚠️ this parameter is intended for use by expert users that know the name of the optimization variables created
> under-the-hood by Antares Simulator.


## Model-wise parameters
see Here : TODO: mettre un lien vers la doc spécifique