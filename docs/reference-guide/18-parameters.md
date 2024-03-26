## Antares Simulator Parameters

### General parameters
These parameters are listed under the `[general]` section in the `.ini` file.  

#### Study mode

##### mode
- **Expected value:** one of the following (case in-sensitive):
  - `economy` or `economic`
  - `adequacy`
  - `expansion`
- **Required:** **yes** TODO
- **Default value:** `economy`
- **Usage:** this parameter sets the study mode for Antares
  - *Economy:* Antares simulator will try to ensure balance between load and generation, while minimizing the economical 
    cost of the grid's operation (more on this [here](01-introduction.md#transmission-project-profitability)).
  - *Adequacy:* in this mode, all power plants' operational cost is considered zero. Antares' only objective is to ensure 
    balance between load and generation (more on this [here](01-introduction.md#generation-adequacy-problems)). 
  - *Expansion:* Antares simulator will optimize the investments on the grid, minimizing both investments and 
    operational costs. If selected, the simulator will TODO

#### Study horizon

##### horizon
- **Expected value:** year (string)
- **Required:** **yes** TODO
- **Default value:** empty string
- **Usage:** the horizon of the study

#### Calendar parameters

##### nbyears
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** number of Monte-Carlo years to simulate

##### simulation.start
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** 0 ? TODO
- **Usage:** index of first day to include in study (>= 0)

##### simulation.end
- **Expected value:** unsigned integer
- **Required:** **yes** TODO
- **Default value:** 365 ? TODO
- **Usage:** index of last day to include in study (<= 365)

##### january.1st
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`, 
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`, 
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday` TODO
- **Usage:** this parameter should indicate the day-of-week of january 1st of the given [year](#horizon).

##### first-month-in-year
- **Expected value:** one of the following string (case-insensitive): `jan`/`january`, `feb`/`february`, `mar`/`march`, 
  `apr`/`april`, `may`, `jun`/`june`, `jul`/`july`, `aug`/`august`, `sep`/`september`, `oct`/`october`, 
  `nov`/`november`, `dec`/`december`.
- **Required:** **yes**
- **Default value:** TODO
- **Usage:** this is the first month in the studied [year](#horizon).

##### first.weekday
- **Expected value:** string carrying an integer between 0 and 6, or one of (case-insensitive): `monday`/`lundi`,
  `tuesday`/`mardi`, `wednesday`/`mercredi`, `thursday`/`jeudi`, `friday`/`vendredi`, `saturday`/`samedi`,
  `sunday`/`dimanche`.
- **Required:** no
- **Default value:** `monday`
- **Usage:** this is the day-of-week of the first day in the studied [horizon](#horizon).

##### leapyear
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** indicates if the studied [year](#horizon) is a leap-year.

#### Additional parameters

##### year-by-year
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** 
  - `true`: results will be exported on a yearly basis
  - `false`: results will be all in one file TODO

##### derated
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO  
  *Note: this parameter cannot be used with [custom-scenario](#custom-scenario)*

##### custom-scenario
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO  
  *Note: this parameter cannot be used with [derated](#derated)*  
  *Note: if set to `true`, [active-rules-scenario](#active-rules-scenario) must be set*

##### user-playlist
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` in order to select specific Monte-Carlo years to simulate. 
  Use the [playlist parameters](#playlist-parameters) to select these years.  
  *Note: this parameter cannot be used with [derated](#derated)*

##### thematic-trimming
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** set to `true` in order to select a specific subset of the optimization variables to print in the output, 
  using a thematic filter. Use the [variables selection parameters](#variables-selection-parametrs) to define the filter.

##### geographic-trimming
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO

##### active-rules-scenario
- **Expected value:** undefined, or `default ruleset` (case-insensitive)
- **Required:** no, unless [custom-scenario](#custom-scenario) is set to `true`
- **Default value:** undefined
- **Usage:** TODO defines how scenarios are built. Only one supported value for now:
  - `default ruleset`: TODO

### Time-series parameters

##### generate
- **Expected value:** `import timeseries`, or a comma-seperated list of 1 to N elements among the following: `mc`, 
  `output`, `load`, `solar`, `wind`, `hydro`, `thermal`
  (ex: `generate = load, hydro, wind`)
- **Required:** no
- **Default value:** empty
- **Usage:** if set to `import timeseries`, all the time-series will be imported from input files. Else, the time-series 
  listed in this parameter will be generated randomly by the simulator.  
  *Note: time-series generation is not available for transmission capacities (NTC)*

##### nbtimeseriesload
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `load` time-series are [automatically generated](#generate), this parameter sets the number of different 
  time-series to generate.

##### nbtimeserieshydro
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `hydro` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

##### nbtimeserieswind
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `wind` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

##### nbtimeseriesthermal
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `thermal` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

##### nbtimeseriessolar
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `solar` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

##### refreshtimeseries
- **Expected value:** `import timeseries`, or a comma-seperated list of 1 to N elements among the following: `mc`,
  `output`, `load`, `solar`, `wind`, `hydro`, `thermal`
  (ex: `generate = load, hydro, wind`)
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter lists those of them that have 
  to be refreshed on-line ?? TODO  
  *Note: time-series refresh is not available for transmission capacities (NTC)*

##### intra-modal
- **Expected value:** empty, or a comma-seperated list of 1 to N elements among the following: `mc`,
  `output`, `load`, `solar`, `wind`, `hydro`, `thermal`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter lists those of them that are 
  correlated, i.e. for which the same time-series should be used together in a given scenario. ?? TODO  
  *Note: inter-modal correlation is not available for transmission capacities (NTC)*  
  *Note: this is the historical correlation mode*

##### inter-modal
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### refreshintervalload
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `load` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this 
  parameter sets their refresh interval (in number of Monte-Carlo years).

##### refreshintervalhydro
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `hydro` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

##### refreshintervalwind
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `wind` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

##### refreshintervalthermal
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `thermal` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

##### refreshintervalsolar
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `solar` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

### Pre-processor parameters

##### readonly
- **Expected value:** `true` or `false`
- **Required:** no
- **Default value:** `false`
- **Usage:** TODO

### Input parameters
These parameters are listed under the `[input]` section in the `.ini` file.

##### import
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

### Output parameters
These parameters are listed under the `[output]` section in the `.ini` file.

##### synthesis
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### storenewest
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### archives
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

### Optimization parameters
These parameters are listed under the `[optimization]` section in the `.ini` file.

##### simplex-range
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### transmission-capacities
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### link-type
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-constraints
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-hurdlecosts
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-tc-minstablepower
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-tc-min-ud-time
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-dayahead
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-strategicreserve
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-spinningreserve
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-primaryreserve
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-exportmps
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-split-exported-mps
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-exportstructure
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### include-unfeasible-problem-behavior
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO


### Adequacy-patch parameters
These parameters are listed under the `[adequacy patch]` section in the `.ini` file.

##### include-adq-patch
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### set-to-null-ntc-from-physical-out-to-physical-in-for-first-step
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### set-to-null-ntc-between-physical-out-for-first-step
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO


### Other parameters
These parameters are listed under the `[other preferences]` section in the `.ini` file.

##### initial-reservoir-levels
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### hydro-heuristic-policy
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### hydro-pricing-mode
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### power-fluctuations
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### shedding-policy
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### unit-commitment-mode
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### number-of-cores-mode
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### renewable-generation-modelling
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### day-ahead-reserve-management
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO


### Advanced parameters
These parameters are listed under the `[advanced parameters]` section in the `.ini` file.

##### accuracy-on-correlation
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### adequacy-block-size
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO


### Seeds - Mersenne Twister parameters
These parameters are listed under the `[seeds - Mersenne Twister]` section in the `.ini` file.

##### seed-tsgen-wind
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-tsgen-load
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-tsgen-hydro
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-tsgen-thermal
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-tsgen-solar
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-tsnumbers
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-unsupplied-energy-costs
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-spilled-energy-costs
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-thermal-costs
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-hydro-costs
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO

##### seed-initial-reservoir-levels
- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO





### Playlist parameters
These parameters are listed under the `[playlist]` section in the `.ini` file.  
TODO

### Variables selection parameters
These parameters are listed under the `[variables selection]` section in the `.ini` file.  
TODO

### Model-wise parameters
see Here : TODO: mettre un lien vers la doc spécifique



- **Expected value:** TODO
- **Required:** **yes** TODO
- **Default value:** TODO
- **Usage:** TODO