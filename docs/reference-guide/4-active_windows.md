
# Active windows

Data can be reviewed, updated, deleted by selecting different possible active windows whose list and content 
are described hereafter. On launching Antares, the default active window is "System Maps".

## System Maps

This window is used to define the general structure of the system, i.e. the list of areas and that of the interconnections. Only the area's names, location and the topology of the grid are defined at this stage. Different colors may be assigned to different areas. These colors may later be used as sorting options in most windows. They are useful to edit data in a fashion that has a geographic meaning (which the lexicographic order may not have). This window displays copy/paste/select all icons equivalent to the relevant EDIT menu commands.

The top left side of the window shows a "mouse status" field with three icons. These icons (one for nodes, one for links and one for binding constraints) indicate whether a selection made on the map with the mouse will involve or not the related elements. When a copy/paste action is considered, this allows for instance to copy any combination of nodes, links and binding constraints. Status can be changed by toggling the icons. Default is "on" for the three icons. Two other purely graphic icons/buttons (no action on data) allow respectively to center the map on a given set of (x , y) coordinates, and to prune the "empty" space around the current map. Multiple additional maps may be defined by using the cross-shaped button located top right. A detailed presentation of all system map editor features can be found in the document "System Map Editor Reference Guide".

## Simulation

The main simulation window is divided up in two parts. On the left side are the general parameters while the right side is devoted to the time-series management.

These two parts are detailed hereafter.

### LEFT PART: General parameters

- **Simulation**

    - _Mode:_ Economy, Adequacy, Draft [^5]
    - _First day:_ First day of the simulation (e.g. 8 for a simulation beginning on the second week of the first 
  month of the year)
    - _Last day:_ Last day of the simulation (e.g. 28 for a simulation ending on the fourth week of the first month
  of the year) [^6]

- **Calendar**

    - _Horizon:_ Reference year (static tag, not used in the calculations)

    - _Year:_ Actual month by which the Time-series begin (Jan to Dec, Oct to Sep, etc.)

    - _Leap Year:_ (Yes/No) indicates whether February has 28 or 29 days

    - _Week:_ In economy or adequacy simulations, indicates the frame (Mon- Sun, Sat-Fri, etc.) to use for
  the edition of weekly results

    - _1st January:_ First day of the year (Mon, Tue, etc.)

- **Monte-Carlo scenarios**

     - _Number:_ Number of MC years that should be prepared for the simulation (not always the same as the
  Number of MC years actually simulated, see "selection mode" below)

     - _Building mode_:
         - **Automatic** For all years to simulate, all time-series will be drawn at random
         - **Custom** The simulation will be carried out on a mix of deterministic and probabilistic conditions,
       with some time-series randomly drawn and others set to user-defined values. This option allows setting
       up detailed "what if" simulations that may help to understand the phenomena at work and quantify various kinds of risk indicators. To set up the simulation profile, choose in the main menu: Configure/ MC scenario builder
         - **Derated** All time-series will be replaced by their general average and the number of MC years
       is set to 1. If the TS are ready-made or Antares-generated but are not to be stored in the INPUT folder,
       no time-series will be written over the original ones (if any). If the time-series are built by Antares
       and if it is specified that they should be stored in the INPUT, a single average-out time series will be stored
       instead of the whole set.

     - _Selection mode_:
         - **Automatic** All prepared MC years will actually be simulated.

         - **Custom** The years to simulate are defined in a list. To set up this list, choose in the main menu:
       Configure/ MC scenario playlist [^7].

- **Output profile**

    - Simulation synthesis:
        - **True** Synthetic results will be stored in a directory:  
      `Study_name/OUTPUT/simu_tag/Economy/mc-all`
        - **False** No general synthesis will be printed out

    - Year-by-Year: 
        - **False** No individual results will be printed out
        - **True** For each simulated year, detailed results will be printed out in an individual directory [^8]:  
      `Study_name/OUTPUT/simu_tag/Economy/mc-i-number

    - Geographic Trimming:
        - **None** Storage of results for all areas, geographic districts, interconnections as well as all time spans
      (hourly, daily, etc.)
        - **Custom** Storage of the results selected with the "Geographic Trimming" command of the "Configure"
      option available in the main menu.  
      Filters on areas, interconnections and time spans may also be defined as follows:
            - On the map, select area(s) and/or interconnection(s)
            - Open the inspector module (Main menu, Windows)
            - Set adequate parameters in the "output print status" group

    - Thematic Trimming:
        - **None** Storage, for the geographic selection defined previously,
      of all variables defined in [Output Files](5-output_files.md) for Areas and Links.
        - **Custom** Storage, for the geographic selection defined previously, of the variables selected with
      the "Thematic Trimming" command of the "Configure" option available in the main menu.

    - MC Scenarios:
        - **False** No storage of the time-series numbers (either randomly drawn or user-defined) used to
      set up the simulation
        - **True** A specific OUTPUT folder will be created to store all the time-series numbers drawn when
        - preparing the MC years.

### RIGHT PART: Time-series management

For the different kinds of time-series that Antares manages in a non-deterministic way (load, thermal generation, hydro power, wind power, solar power or renewable depending on the option chosen):

1. **Choice of the kind of time-series to use**  
Either « ready-made » or «stochastic » (i.e. Antares-generated), defined by setting the value to either "on" or "off". Note that for Thermal time-series, the cluster-wise parameter may overrule this global parameter (see Thermal window description below).

2. **For stochastic TS only**:
    - **Number** Number of TS to generate

    - **Refresh** (Yes /No) ndicates whether a periodic renewal of TS should be performed or not

    - **Refresh span** Number of MC years at the end of which the renewal will be performed (if so required)

    - **Seasonal correlation** ("monthly" or "annual") Indicates whether the spatial correlation matrices to use are defined month by month or if a single annual matrix for the whole year should rather be used (see [Time-series analysis and generation](6-time_series_analysis_and_generation.md))

    - **Store in input**
        - **Yes** the generated time-series will be stored in the INPUT in replacement of the original ones (wherever they may come from)
        - No: the original time-series will be kept as they were

    - **Store in output**
        - **Yes**: the generated times-series will be stored as part of the simulation results
        - **No**: no storage of the generated time-series in the results directories

3. **General rules for building up the MC years**
    - **Intra-modal**:
        - **Yes** For each mode, the same number should be used for all locations (or 1 where there is only one TS), but this number may differ from one mode to another. For instance, solar power TS = 12 for all areas, while wind power TS number = 7 for all areas.
        - **No** Independent draws
    - **Inter-modal**:
           - **Yes** For all modes, the same number should be used but may depend on the location (for instance, solar and wind power TS = 3 for area 1, 8 for area 2, 4 for area 3, etc.)
           - **No** Independent draws

A full meteorological correlation (for each MC year, one single number for all modes and areas) is, from a theoretical standpoint, accessible by activating "intra-modal" and " inter-modal" for all but the "thermal" kind of time-series. The availability of an underlying comprehensive multi-dimensional Meteorological data base of ready-made time-series is the crux of the matter when it comes to using this configuration.

## User's Notes

A built-in notepad for recording comments regarding the study. Such comments typically help to track successive input data updates (upgrading such interconnection, removing such plant, etc.). Another simple use is to register what has been stored in the "user" subfolder and why. Such notes may prove useful to sort and interpret the results of multiple simulations carried out at different times on various configurations of the power system.

## Load

This window is used to handle all input data regarding load. In Antares load should include transmission losses. It should preferably not include the power absorbed by pumped storage power plants. If it does, the user should neither use the "PSP" array (see window "Misc. Gen") nor the explicit modeling of PSP plants

The user may pick any area appearing in the list and is then given access to different tabs:

- The "time-series" tab display the "ready-made" 8760-hour time-series available for simulation purposes. These data may come from any origin outside Antares, or be data formerly generated by the Antares time-series stochastic generator, stored as input data on the user's request. Different ways to update data are :

    - direct typing

    - copy/paste a selected field to/from the clipboard

    - load/save all the time-series from/to a file (usually located in the "user" subfolder)

    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values
  (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")

    - Handle the whole (unfiltered) existing dataset to either

        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

    Versatile "Filter" functions allow quick access to user-specified sections of data
(e.g. display only the load expected in the Wednesdays of January, at 09:00, for time-series #12 to #19). Hourly load is expressed in round numbers and in MW. If a smaller unit has to be used, the user should define accordingly ALL the data of the study (size of thermal plants, interconnection capacities, etc.)

    - _Note that:_
    
        - _If the "intra-modal correlated draw" option has not been selected in the_ **simulation** _window,
      MC adequacy or economy simulations can take place even if the number of time-series is not the same
      in all areas (e.g. 2 , 5 , 1 , 45 ,...)_
        - _If the "intra-modal correlated draws" option has been selected in the_ **simulation** _window,
      every area should have either one single time-series or the same given number (e.g. 25 , 25 , 1 , 25...)_
        
    - The "spatial correlation" tab gives access to the inter-area correlation matrices that will be used
    by the stochastic generator if it is activated. Different sub-tabs are available for the definition of 12 monthly
    correlation matrices and of an overall annual correlation matrix.
    A matrix A must meet three conditions to be a valid correlation matrix:
      $$\forall i,\ \forall j,\ {A_{ii} = 100; -100 \le A_{ij} \le 100}; A\ symmetric; A\ positive\ semi\mbox{-}definite$$
      When given invalid matrices, the TS generator emits an infeasibility diagnosis

    - The "local data" tab is used to set the parameters of the stochastic generator.
    These parameters are presented in four sub-tabs whose content is presented in
    [Time-series analysis and generation](6-time_series_analysis_and_generation.md).

    - The "digest" tab displays for all areas a short account of the local data

## Thermal

This window is used to handle all input data regarding thermal dispatchable power.

The user may pick any area appearing in the area list and is then given access to the list of thermal plants
clusters defined for the area (e.g. "CCG 300 MW", "coal 600", etc.). Once a given cluster has been selected, 
a choice can be made between different tabs:

- The "time-series" tab displays the "ready-made" 8760-hour time-series available for simulation purposes. 
These data may come from any origin outside Antares, or be data formerly generated by the Antares time-series 
stochastic generator, stored as input data on the user's request. Different ways to update data are :

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values (e.g. simulate a
  2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either

       - Change the number of columns (function name: resize)
       - Adjust the values associated with the current first day of the year (function name: shift rows)

    Versatile "Filter" functions allow quick access to user-specified sections of data (e.g. display only
the generation expected on Sundays at midnight, for all time-series).

    Hourly thermal generation is expressed in round numbers and in MW. If a smaller unit has to be used, the user should define accordingly ALL the data of the study (Wind generation, interconnection capacities, load, hydro generation, solar, etc.)

    - _Note that:_

        - _If the "intra-modal correlated draws" option has not been selected in the_ **simulation** _window,
      MC adequacy or economy simulations can take place even if the number of time-series is not the same in all
      areas (e.g. 2, 5, 1, 45,etc.)_

        - _If the "intra-modal correlated draws" option has been selected in the_ **simulation** _window,
      every area should have either one single time-series or the same given number (e.g. 25, 25, 1, 25, etc.).
      Note that, unlike the other time-series (load, hydro, etc.), which depend on meteorological conditions and
      are therefore inter-area-correlated, the thermal plants time-series should usually be considered as uncorrelated.
      Using the "correlated draws" feature makes sense only in the event of having to play predefined scenarios
      (outside regular MC scope)_


- The "TS generator" tab is used to set the parameters of the stochastic generator.
These parameters are defined at the daily scale and are namely, for each day: the average duration of
forced outages (beginning on that day), the forced outage rate, the duration of planned outages (beginning on that day),
the planned outage rate, planned outages minimum and maximum numbers.
Durations are expressed in days and rates belong to [0 , 1].


- The "Common" tab is used to define the cluster's techno-economic characteristics :

    - Name
    - Fuel used
    - Location (Area)
    - Activity status
        - false: not yet commissioned, moth-balled, etc.
        - true : the plant may generate
    - Number of units
    - Nominal capacity
    - Full Must-run status
        - false: above a partial "must-run level" (that may exist or not, see infra) plants
      will be dispatched on the basis of their market bids.
        - true: plants will generate at their maximum capacity, regardless of market conditions
    - Minimum stable power (MW)
    - Minimum Up time (hours)
    - Minimum Down time (hours)
    - Default contribution to the spinning reserve (% of nominal capacity)
    - CO2 tons emitted per electric MWh
    - Marginal operating cost (€/MWh)
    - Volatility (forced): a parameter between 0 and 1, see section [Time-series generation (thermal)](6-time_series_analysis_and_generation.md#time-series-generation-thermal)
    - Volatility (planned): a parameter between 0 and 1, see section [Time-series generation (thermal)](6-time_series_analysis_and_generation.md#time-series-generation-thermal)
    - Law (forced): Probabilistic law used for the generation of the forced outage time-series, can be set to either uniform or geometric
    - Law (planned): Probabilistic law used for the generation of the planned outage time-series, can be set to either uniform or geometric
    - Generate TS: Parameter to specify the behavior of this cluster for TS generation. **This cluster-wise parameter takes priority over the study-wide one.** It can hold three values:
      - Force generation: TS for this cluster will be generated
      - Force no generation: TS for this cluster will not be generated
      - Use global parameter: Will use the parameter for the study (the one in the [Simulation Window](#simulation)).
    - Fixed cost (No-Load heat cost) (€ / hour of operation )
    - Start-up cost (€/start-up)
    - Market bid (€/MWh)
    - Random spread on the market bid (€/MWh)
    - Seasonal marginal cost variations (gas more expensive in winter, ...)
    - Seasonal market bid modulations (assets costs charging strategy )
    - Nominal capacity modulations (seasonal thermodynamic efficiencies, special over-generation allowances, etc.). These modulations are taken into account during the generation of available power time-series
    - Minimal generation commitment (partial must-run level) set for the cluster

    - _Note that:_

        - _The **optimal dispatch plan** as well as **locational marginal prices** are based on **market bids**,
      while the assessment of the **operating costs** associated with this optimum are based on **cost parameters**.
      (In standard "perfect" market modeling, there is no difference of approaches because market
      bids are equal to marginal costs)_

## Hydro

This section of the GUI is meant to handle all input data regarding hydro power,
as well as any other kind of energy storage system of any size (from a small battery to a large 
conventional hydro-storage reservoir with or without pumping facilities, etc.): Hydro power being 
historically the first and largest form of power storage, it stood to reason that it should play in 
Antares the role of a "generic template" for all forms of storable power. This versatility, however, 
comes at the price of a comparatively more complex data organization than for other objects, 
which explains the comparatively long length of this chapter.

In the main Window, the user may pick any area appearing in the list and is then given access to different tabs:

- The "time-series" tab displays the "ready-made" time-series already available for simulation purposes. There are two categories of time-series (displayed in two different subtabs): the Run of River (ROR) time-series on the one hand and the Storage power (SP) time-series on the other hand.

    ROR time-series are defined at the hourly scale; each of the 8760 values represents the ROR power expected at a given hour, expressed in round number and in MW. The SP time-series are defined at the daily scale; each of the 365 values represents an overall SP energy expected in the day, expressed in round number and in MWh. These natural inflows are considered to be storable into a reservoir for later use.
  
    Both types of data may come from any origin outside Antares, or may have been formerly generated by the Antares time-series stochastic generator and stored as input data on the user's request. Different ways to update data are:

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values
  (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either

        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

    - _Note that:_

        - _For a given area, the number of ROR time-series and SP times-series **must** be identical_
        - _If the "intra-modal correlated draws" option was not selected in the_ **simulation** _window,
      MC adequacy or economy simulations can take place even if the number of hydro time-series is not the same 
      in all areas (e.g. 2 , 5 , 1 , 45 ,...)_
        - _If the "intra-modal correlated draws" option was selected in the_ **simulation** _window, every 
      area should have either one single time-series or the same given number (e.g. 25 , 25 , 1 , 25...)_
    

- The "spatial correlation" tab gives access to an annual inter-area correlation matrix that will be used by the stochastic generator if it is activated. Correlations are expressed in percentages, hence to be valid this matrix must be symmetric, p.s.d, with a main diagonal of 100s and all terms lying between (-100 ,+100)


- The "Allocation" tab gives access to an annual inter-area allocation matrix A(i,j) that may be used during a heuristic hydro pre-allocation process, regardless of whether the stochastic time-series generator is used or not. This matrix describes the weights that are given to the loads of areas (i) in the definition of the monthly and weekly hydro storage generation profiles of areas (j). The way this is done in detailed in [Miscellaneous](8-miscellaneous.md).


- The "local data" tab is used to set up the parameters of the stochastic generator_ **AND** _to define techno-economic characteristics of the hydro system that are used in Economy and Adequacy optimizations. For the purpose of versatility (use of the hydro section to model storage facilities quite different in size and nature), this "local tab" is itself divided up into four different subtabs whose list follows and which are further described:

    - Inflow Structure
    - Reservoir Levels and water value
    - Daily Power and Energy Credits
    - Management options

**Inflow Structure**

This tab contains all the local parameters used for the stochastic generation of hydro time-series. These are namely:

- The expectations, standard deviations, minimum and maximum values of monthly energies (expressed in MWh), monthly shares of Run of River within the overall hydro monthly inflow.
- The average correlation between the energy of a month and that of the next month (inter-monthly correlation).
- The average daily pattern of inflows within each month. Each day is given a relative "weight" in the month. If all days are given the same weight, daily energy time-series will be obtained by dividing the monthly energy in equal days. If not, the ratio between two daily energies will be equal to that of the daily weights in the pattern array.

Overall hydro energy is broken down into two parts: Run of River- ROR and storage -STOR

ROR energy has to be used on the spot, as it belongs to the general "must-run" energy category.

STOR energy can be stored for use at a later time. The way how stored energy may actually be used depends 
on the options chosen in the "management options" Tab and of the values of the parameters defined in the other Tabs.

**Reservoir Levels and Water Values**

Reservoir levels (left side)

On the left side are defined 365 values for the minimum, average and maximum levels set for the reservoir 
at the beginning of each day, expressed in percentage of the overall reservoir volume. The lower and upper level 
time-series form a pair of so-called lower and upper "reservoir rule curves"

Depending on the set of parameters chosen in the "management options" Tab, these rule curves may be used in 
different ways in the course of both heuristic seasonal hydro pre-allocation process and subsequent weekly 
optimal hydro-thermal unit-commitment and dispatch process.

Water values (right side)

On the right side is a table of marginal values for the stored energy, which depends on the date (365 days) and 
of the reservoir level (101 round percentage values ranging from 0% to 100%). These values may have different origins; 
they theoretically should be obtained by a comprehensive (dual) stochastic dynamic programming study carried out over 
the whole dataset and dealing simultaneously with all reservoirs.

Depending on the set options chosen in the "management options" Tab, these values may be used or not in the course of 
the weekly optimal hydro-thermal unit-commitment and dispatch process.

**Daily Power and Energy Credits**

Standard credits (Bottom part)

The bottom part displays two daily time-series (365 values) defined for energy generation/storage 
(hydro turbines or hydro pumps). In each case, the first array defines the maximum power (generated or absorbed), 
and the second defines the maximum daily energy (either generated or stored).

For the sake of clarity, maximum daily energies are expressed as a number of hours at maximum power.

Credit modulation (Upper part)

The upper part displays two level-dependent (101 round percentage values ranging from 0% to 100%) time-series 
of modulation coefficients defined for either generating or storing (pumping).

These modulations, which can take any positive value, may (depending on the options chosen in the management 
options Tab) be used to increase (value &gt;1) or to decrease (value &lt;1) the standard credits defined previously 
for the maximum daily power and energies.

**Management Options**

This Tab is a general dashboard for the definition of how storage units, whatever their size or nature, should be managed. 
It includes 15 parameters (out of which 7 are booleans) presented hereafter:

- "Follow load" (y|n): defines whether an "ideal" seasonal generation profile should somehow follow the load OR an 
"ideal" seasonal generation profile should remain as close as possible to the natural inflows 
(i.e. instant generation whenever possible)

- "Inter-daily breakdown" and "Inter-monthly breakdown" : parameters used in the assessment, through a 
heuristic process, of an "ideal" seasonal generation profile, if the use of such a profile is required 
(the heuristic itself is presented in [Miscellaneous](8-miscellaneous.md))

- "Intra-daily modulation": parameter which represents, for the storage power, the maximum authorized value for
the ratio of the daily peak to the average power generated throughout the day. This parameter is meant to allow 
simulating different hydro management strategies. Extreme cases are : 1 : generated power should be constant throughout 
the day 24 : use of the whole daily energy in one single hour is allowed

- "Reservoir management" `y|n`: defines whether the storage should be explicitly modeled or not.

    Choosing "No" implies that available data allow or require that, regardless of the reservoir characteristics:
    
	- The whole amount of STOR energy of each month MUST be used during this month (no long-term storage) 
	
	- The actual daily generation should follow, during the month, an "ideal" profile defined by the heuristic defined in [Miscellaneous](8-miscellaneous.md)

 	Choosing "Yes" implies that available data allow or require explicit modeling of the storage facility,
regardless of whether a pre-allocation heuristic is used or not.

- "Reservoir capacity": size of the storage facility, in MWh

- "Initialize reservoir level on the 1<sup>st</sup> of": date at which the reservoir level should be initialized by a random draw. The "initial level" is assumed to follow a "beta" variable with expectation "average level", upper bound U=max level, lower bound L= min level, standard deviation = (1/3) (U-L)

- "Use Heuristic Target" (y|n): defines whether an "ideal" seasonal generation profile should be heuristically determined or not.

    Choosing "No" implies that available data allow or require that full confidence should be put in water values determined upstream (through [dual] stochastic dynamic programming) OR that there are no "natural inflows" to the storage facility (battery or PSP, etc.)

    Choosing "Yes" implies that available data allow or require the definition of an "ideal" generation profile, that can be used to complement –or replace– the economic signal given by water values AND that there are "natural inflows" on which a generation heuristic can be based.

- "Power-to-Level modulations (y|n)": defines whether the standard maximum daily energy and power credit should be or not multiplied by level-dependent modulation coefficients.

- "Hard bounds on rule curves (y|n)": states whether, beyond the preliminary heuristic stage (if any), lower and upper reservoir rule curves should still be taken into account as constraints in the hydro-thermal unit commitment and dispatch problems.

- "Use leeway (y|n)", lower bound L, upper bound U: states whether the heuristic hydro ideal target (**HIT**) should be followed exactly or not.

    Choosing "No" implies that, in optimization problems, the hydro energy generated throughout the time interval will be subject to an equality constraint, which may include short-term pumping cycles independent of water value: sum{ 1,t,T} (hydro(t)) – sum{1,t,T} (r. pump(t))= **HIT*

    Choosing "Yes", with bounds L and U, implies that, in optimization problems, the hydro energy generated throughout the time span will be subject to inequality constraints: L_*__HIT__ _&lt;=sum{1,t,T} (hydro(t)) &lt;= U\*_**HIT*

    Independently, short- or long-term pumping may also take place if deemed profitable in the light of water values.

  - "Use Water Value (y|n)": states whether the energy taken from / stored into the reservoir should be given the reference value defined in the ad hoc table OR should be given a zero value.

  - "Pumping Efficiency Ratio": setting the value to r means that, for the purpose of storing 1 gravitational MWh, pumps will have to use (1/r) electrical MWh.

## Wind

This window is used to handle all input data regarding Wind power.
This window is only accessible when the advanced parameter Renewable Generation modeling is set to "Aggregated".

The user may pick any area appearing in the list and is then given access to different tabs:

- The "time-series" tab display the "ready-made" 8760-hour time-series already available for simulation purposes. These data may come from any origin outside Antares, or be data formerly generated by the Antares time-series stochastic generator, stored as input data on user's request. Different ways to update data are :

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either
        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

    Versatile "Filter" functions allow quick access to user-specified sections of data (e.g. display only the wind generation expected between 17:00 and 21:00 in February, for time-series 1 to 100).

    Hourly wind generation is expressed in round numbers and in MW. If a smaller unit has to be used, the user should define accordingly ALL the data of the study (size of thermal plants, interconnection capacities, load, etc.)

    - _Note that:_

        - _If the "intra-modal correlated draws" option has not been selected in the_ **simulation** _window, MC adequacy or economy simulations can take place even if the number of time-series is not the same in all areas (e.g. 2, 5, 1,45, ...)_

        - _If the "intra-modal correlated draws" option has been selected in the_ **simulation** _window, every area should have either one single time-series or the same given number (e.g. 25, 25, 1, 25, ...)_

- The "spatial correlation" tab gives access to the inter-area correlation matrices that will be used by the stochastic generator if it is activated. Different sub-tabs are available for the definition of 12 monthly correlation matrices and an overall annual correlation matrix.
  
  A matrix A must meet three conditions to be a valid correlation matrix:
    $$\forall i,\ \forall j,\ {A_{ii} = 100; -100 \le A_{ij} \le 100}; A\ symmetric; A\ positive\ semi\mbox{-}definite$$
    When given invalid matrices, the TS generator emits an infeasibility diagnosis

- The "local data" tab is used to set the parameters of the stochastic generator. These parameters are presented in four subtabs whose content is presented in [Time-series analysis and generation](6-time_series_analysis_and_generation.md).

- The "digest" tab displays for all areas a short account of the local data


## Solar

This window is used to handle all input data regarding Solar power. Both thermal solar generation and PV solar generation are assumed to be bundled in this data section.  
_This window is only accessible when the advanced parameter Renewable Generation modeling is set to "aggregated”._

The user may pick any area appearing in the list and is then given access to different tabs:

- The "time-series" tab display the "ready-made" 8760-hour time-series available for simulation purposes. These data may come from any origin outside Antares, or be data formerly generated by the Antares time-series stochastic generator, stored as input data on the user's request. Different ways to update data are :

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either

        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

    Versatile "Filter" functions allow quick access to user-specified sections of data (e.g. display only the solar power expected in August at noon, for all time-series).

    Hourly solar power is expressed in round numbers and in MW. If a smaller unit has to be used, the user should define accordingly ALL the data of the study (size of thermal plants, interconnection capacities, etc.)

    - _Note that:_

        - _If the "intra-modal correlated draws" option was not selected in the_ **simulation** _window, MC adequacy or economy simulations can take place even if the number of time-series is not the same in all areas (e.g. 2 , 5 , 1 , 45 ,...)_

        - _If the "intra-modal correlated draws" option was selected in the_ **simulation** _window, every area should have either one single time-series or the same given number (e.g. 25 , 25 , 1 , 25...)_

- The "spatial correlation" tab gives access to the inter-area correlation matrices that will be used by the stochastic generator if it is activated. Different sub-tabs are available for the definition of 12 monthly correlation matrices and of an overall annual correlation matrix.

    A matrix A must meet three conditions to be a valid correlation matrix:

  $$\forall i,\ \forall j,\ {A_{ii} = 100; -100 \le A_{ij} \le 100}; A\ symmetric; A\ positive\ semi\mbox{-}definite$$
    When given invalid matrices, the TS generator emits an infeasibility diagnosis


- The "local data" tab is used to set the parameters of the stochastic generator. These parameters are presented in four subtabs whose content is presented in [Time-series analysis and generation](6-time_series_analysis_and_generation.md).

- The "digest" tab displays for all areas a short account of the local data


## Renewable

This window is used to handle all input data regarding renewable generation.  
_This window is only accessible when the advanced parameter Renewable Generation modeling is set to "cluster” (default value)._

The user may pick any area appearing in the area list and is then given access to the list of renewable clusters defined for the area (e.g. "Onshore Wind Farm 200MW", "Solar Rooftop 50MW", etc.). Once a given cluster has been selected, a choice can be made between different tabs:

- The "time-series" tab displays the "ready-made" 8760-hour time-series available for simulation purposes. These data may come from any origin outside Antares, or be data formerly generated by the Antares time-series stochastic generator, stored as input data on the user's request. Different ways to update data are :

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either

        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

 	Versatile "Filter" functions allow quick access to user-specified sections of data (e.g. display only the generation expected on Sundays at midnight, for all time-series).

	Hourly thermal generation is expressed in round numbers and in MW. If a smaller unit has to be used, the user should define accordingly ALL the data of the study (Wind generation, interconnection capacities, load, hydro generation, solar, etc.)

    - _Note that:_

        - _If the "intra-modal correlated draws" option has not been selected in the_ **simulation** _window, MC adequacy or economy simulations can take place even if the number of time-series is not the same in all areas (e.g. 2, 5, 1, 45,etc.)_

        - _If the "intra-modal correlated draws" option has been selected in the_ **simulation** _window, every area should have either one single time-series or the same given number (e.g. 25, 25, 1, 25, etc.). Note that, unlike the other time-series (load, hydro, etc.), which depend on meteorological conditions and are therefore inter-area-correlated, the thermal plants time-series should usually be considered as uncorrelated. Using the "correlated draws" feature makes sense only in the event of having to play predefined scenarios (outside regular MC scope)_


- The "TS generator" tab is not accessible for this version.


- The "Common" tab is used to define the cluster's techno-economic characteristics :

    - Name
    - Group: The group can be any one of the following: Wind Onshore, Wind Offshore, Solar Thermal, Solar PV, Solar Rooftop, Other RES 1, Other RES 2, Other RES 3, or Other RES 4. If not specified, the renewable cluster will be part of the group Other RES 1.
    - Location (Area)
    - Timeseries mode:
        - Power generation means that the unit of the timeseries is in MW
        - Production factor means that the unit of the timeseries is in p.u. (between 0 and 1, 1 meaning the full installed capacity)
    - Activity status
        - false: not yet commissioned, moth-balled, etc.
        - true: the cluster may generate
    - Number of units
    - Nominal capacity (in MW per unit)


## Misc. Gen.

This window is used to handle all input data regarding miscellaneous non dispatchable generation.

On picking any area in the primary list, the user gets direct access to all data regarding the area, which amount to **8** ready-made 8760-hour time-series (expressed in MW):

- CHP generation

- Bio Mass generation

- Bio-gas generation

- Waste generation

- Geothermal generation

- Any other kind of non-dispatchable generation

- A predefined time-series for the operation of Pumped Storage Power plants, if they are not explicitly modeled. A positive value is considered as an output (generating) to the grid, a negative value is an input (pumping) to the station.

  Note that the sum of the 8760 values must be negative, since the pumping to generating efficiency is lower than 1. The user may also use only the negative values (prescribed pumping), while transferring at the same time the matching generating credit on the regular hydro storage energy credit.

- ROW balance: the balance with the rest of the world. A negative value is an export to ROW, a positive value is an import from ROW. These values acts as boundary conditions for the model. Different ways to update data are:

    - direct typing
    - copy/paste a selected field to/from the clipboard
    - load/save all the time-series from/to a file (usually located in the "user" subfolder)
    - Apply different functions (+,-, \*, /,etc.) to the existing (possibly filtered) values (e.g. simulate a 2% growth rate by choosing "multiply-all-by-1.02")
    - Handle the whole (unfiltered) existing dataset to either

        - Change the number of columns (function name: resize)
        - Adjust the values associated with the current first day of the year (function name: shift rows)

## Reserves / DSM

This window is used to handle all input data regarding reserves and the potential of "smart" load management (when not modeled using "fake" thermal dispatchable plants). On picking any area in the primary list, the user gets direct access to all data regarding the area, which amount to **four** ready-made 8760-hour time-series (expressed in MW). The first two are used only in "draft" simulations, while the last two are available in either "adequacy" or "economy" simulations:

- Primary reserve: must be provided whatever the circumstances, even at the price of some unsupplied energy (Draft simulations only)

- Strategic reserve: sets a limit on the backup power that an area is supposed to be able to export to its neighbors. This reserve may represent an actual generation reserve, an energy constraint too complex to model by standard means (e.g. energy policy regarding special reservoirs) or can also be justified by simplifications made in grid modeling. (Draft simulations only).

- Day-ahead reserve: power accounted for in setting up the optimal unit-commitment and schedule of the following day(s), which must consider possible forecasting errors or last-minute incidents. If the optimization range is of one day, the reserve will be actually seen as "day-ahead". If the optimization range is of one week, the need for reserve will be interpreted as "week-ahead". (Adequacy and Economy simulations)

- DSM: power (decrease or increase) to add to the load. A negative value is a load decrease, a positive value is a load increase. Note that an efficient demand side management scheme may result in a negative overall sum (All simulation modes).

## Links

This window is used to handle all input data regarding the interconnections. On picking any interconnection in the primary list, the user gets direct access to all data regarding the link, which are five annual parameters and a set of eight ready-made 8760-hour time-series

The five parameters, used in economy or adequacy simulations (not in draft), are namely:

- "Hurdle cost": set by the user to state whether (linear) transmission fees should be taken into account or not in economy and adequacy simulations
- "Transmission capacities": set by the user to state whether the capacities to consider are those indicated in 8760-hour arrays or if zero or infinite values should be used instead (actual values / set to zero / set to infinite)
- "Asset type": set by the user to state whether the link is either an AC component (subject to Kirchhoff's laws), a DC component, or another type of asset
- "Account for loop flows": set by the KCG [^9] to include (or not) passive loop flows in the formulation of the constraints enforcing Kirchhoff's laws
- "Account for PST": set by the KCG to include (or not) the settings of phase-shifting transformers in the formulation of the constraints enforcing Kirchhoff's laws

The eight 8760-hour times-series are:

- NTC direct: the upstream-to-downstream capacity, in MW

- NTC indirect: the downstream-to-upstream capacity, in MW

- Hurdle cost direct: an upstream-to-downstream transmission fee, in €/MWh

- Hurdle cost indirect: a downstream-to-upstream transmission fee, in €/MWh

- Impedance: used in economy simulations to give a physical meaning to raw outputs, when no binding constraints have been defined to enforce Kirchhoff's laws (see "Output" section, variable "Flow Quad") OR used by the Kirchhoff's constraint generator to build up proper flow constraints (AC flow computed with the classical "DC approximation"). Since voltage levels are not explicitly defined and handled within Antares, all impedances are assumed to be scaled to some reference \\( U_{ref} \\)

- Loop flow: amount of power flowing circularly though the grid when all "nodes" are perfectly balanced (no import and no export). Such loop flows may be expected on any "simplified" grid in which large regions (or even countries) are modeled by a small number of "macro" nodes, and should accordingly be accounted for.

- PST min (denoted \\(Y^-\\) in [Kirchhoff Constraints Generator](7-kirchhoffs_constraint_generator.md)): lower bound of phase-shifting that can be reached by a PST installed on the link, if any (note : the effect of the active loop flow generated by the PST may be superimposed to that of the passive loop flow)

- PST max (denoted \\(Y^+\\) in [Kirchhoff Constraints Generator](7-kirchhoffs_constraint_generator.md)): upper bound of phase-shifting that can be reached by a PST installed on the link, if any (note : the effect of the active loop flow generated by the PST may be superimposed to that of the passive loop flow)

For the sake of simplicity and homogeneity with the convention used for impedance, PST settings are assumed to be expressed in \\( rad/U^2_{ref} \\)

## Binding constraints

This section of the GUI is used to handle all data regarding special constraints that one may wish to include in the formulation of the optimization problems to solve.

The set of tabs described hereafter provides for that purpose all the means required to define arbitrary linear constraints on any subset of continuous variables involved in the modeling of the power system.

Since no limitation is set on the number and content of the constraints that may be defined that way, it is the user's sole responsibility to make sure that these so-called "binding constraints" are realistic and meaningful, be it from a technical or economic standpoint.

A typical situation in which this feature proves useful is, for instance, encountered when data at hand regarding the grid include an estimate of the impedances of the interconnections.

In such cases, assuming that:

- \\(Z_l\\) denotes the impedance of interconnections \\(l=1, L\\)
- A preliminary study of the graph modeling the grid has shown that it can be described by a set of independent meshes \\(c=1, C\\)(cycle basis of the graph)

Then the DC flow approximation may be implemented, for each time-step of the simulation, by a set of C binding constraints between AC flows \\(F_l\\):

$$ c= 1, ..., C : \sum_{i \in C}{sign(l,c)F_lZ_l = 0}$$

_Note that such specific binding constraints can be automatically generated within Antares by using the auxiliary module "Kirchhoff's Constraints Generator" further described in [Kirchhoff Constraints Generator](7-kirchhoffs_constraint_generator.md)._

Aside from such sets of constraints, which may help to give realistic geographic patterns to the flows, completely different sets of constraints may be also defined, such as those set up by the market organization, which may define precise perimeters for valid commercial flows [^10].

More generally, Antares allows to define three categories of binding constraints between transmission flows and/or power generated from generating units:

- "hourly" binding constraints, which are applied to instant power (transmitted and/or generated)

- "daily" binding constraints, that are applied to daily energies. This class makes more sense for commercial modeling (say: imports and exports from/to such and such area should be comprised between such and such lower bound and upper bound). Daily binding constraints are also commonly used to model specific facilities, such as pumped storage units operated on a daily cycle

- "weekly" binding constraints, that are applied to weekly energies. Like the previous ones, these constraints may be used to model commercial contracts or various phenomena, such as the operation of a pumped storage power plant operated on a weekly cycle.

The Binding Constraints section of the GUI involves six main tabs described hereafter:

- **TAB "summary"**  
Creation, edition or removal of a binding constraint. A binding constraint is here defined by four macroscopic attributes that can be set by the edit command:

    - Name (caption)
    - Time-range (hourly, daily, weekly)
    - Numerical type (equality, bounded above, below, on both sides)
    - Status (active /enabled or inactive/disabled)

- **TAB "weights"**  
Definition of the coefficients given to each flow variable or generation variable in the formulation of the constraints. Two sub-tabs make it possible to handle the coefficients associated with transmission assets (links) and those associated with generation assets (thermal clusters). In both cases:

    - The lines of the tables show only the components (links or clusters) that are visible on the current map
    - The columns of the tables show only the constraints that do not have non-zero weights attached to components that are nor visible on the current map

- **TAB "offsets"**  
Definition of the time-lag (in hours) assigned to each flow variable or generation variable in the formulation of the constraints. Two sub-tabs make it possible to handle the offsets associated with transmission assets (links) and those associated with generation assets (thermal clusters). In both cases:

    - The lines of the tables show only the components (links or clusters) that are visible on the current map
    - The columns of the tables show only the constraints that do not have non-zero weights attached to components that are nor visible on the current map

- **TAB "="**  
Definition of the right-hand side of equality constraints. This RHS has either 8760 values (hourly constraints) or 365 values (daily or weekly constraints). Depending on the range actually chosen for the simplex optimization (see section **Configure** of the main menu), the weekly constraints RHS will either be represented by the sum of seven daily terms or by a set of seven daily terms (weekly constraint downgraded to daily status).

- **TAB "&gt;"**  
Definition of the right-hand side of "bounded below" and "bounded on both sides" inequality constraints. This RHS has either 8760 values (hourly constraints) or 365 values (daily or weekly constraints). Depending on the range actually chosen for the simplex optimization (see section **Configure** of the main menu), the weekly constraints RHS will either be represented by the sum of seven daily terms or by a set of seven daily terms (weekly constraint downgraded to daily status).

- **TAB "&lt;"**  
Definition of the right-hand side of "bounded above" and "bounded on both sides" inequality constraints. This RHS has either 8760 values (hourly constraints) or 365 values (daily or weekly constraints). Depending on the range actually chosen for the simplex optimization (see section **Configure** of the main menu), the weekly constraints RHS will either be represented by the sum of seven daily terms or by a set of seven daily terms (weekly constraint downgraded to daily status).

When defining binding constraints between (hourly) power, daily or weekly (energy) flows, special attention should be paid to potential conflicts between them or with the "basic" problem constraints. Lack of caution may result in situations for which the optimization has no solution. Consider for instance a case in which three variables X1, X2, X3 (whatever they physical meaning) are involved in the following binding constraints:

$$X1 + X2 > 5$$

$$X2 < -3$$

$$X3 > 0$$

$$X1 + X3 < 7$$

These commitments are obviously impossible to meet and, if the economic simulator is run on a dataset including such a set of constraints, it will produce an infeasibility analysis that looks like the following.
```
[solver][notic]  Solver: Starting infeasibility analysis...
[solver][error] The following constraints are suspicious (first = most suspicious)
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 124
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 128
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 137
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 140
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 133
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 139
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 136
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 130
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 142
[solver][error] Hydro reservoir constraint at area 'Germany' at hour 123
```

This report should help you identify constraints that generate infeasible linear optimization problems such what is presented above.

The advanced preference "Unfeasible Problems Behavior" gives to the user the ability to choose between four different strategies regarding these situations.

## Economic Opt.

This window is used to set the value of a number of area-related parameters that, aside from the costs of each generating plant, define the optimal solution that Antares has to find in economic simulations. These parameters are namely, for each area of the system:

- The value of the unsupplied energy (also commonly denoted Value Of Lost Load,VOLL) , in €/MWh. This value should usually be set much higher than the cost of the most expensive generating plant of the area

- The random spread within which the nominal unsupplied energy value is assumed to vary

- The value of the spilled energy, in € /MWh. This value reflects the specific penalty that should be added to the economic function for each wasted MWh, if any. Note that even if this value is set to zero no energy will be shed needlessly

- The random spread within which the nominal unsupplied energy value is assumed to vary

- Three parameters named "shedding status" and related to different kinds of generation. If the system cannot be balanced without shedding some generation, these parameters give control on how each kind of generation ("Non dispatchable power","Dispatchable hydropower" and "Other dispatchable generating plants") should contribute to the shedding. Depending on the value chosen for the status, the generation can or cannot be shed to find a solution to the load/generation balance problem. Note that enforcing a negative status for all types of plants may lead to simulations scenarios for which there are no mathematical solutions.

On running the economic simulator, such situations produce an infeasibility diagnosis.


## Miscellaneous

In all previous windows showing Input data, the content can be filtered so as to reflect only items that are associated with Areas and Links defined as "visible" in a particular map. In that regard, binding constraints are considered as visible if and only if all of their non-zero weight associated objects are visible on the map.


[^5]: "Economy" simulations make a full use of Antares optimization capabilities. They require economic as well as technical input data and may demand a lot of computer resources. "Adequacy" simulations are faster and require only technical input data. Their results are limited to adequacy indicators. "Draft" simulations are highly simplified adequacy simulations, in which binding constraints (e.g. DC flow rules) are ignored, while hydro storage is assumed to be able to provide its nominal maximum power whenever needed. As a consequence, draft simulations are biased towards optimism. They are, however, much faster than adequacy and economic simulations.

[^6]: In Economy an Adequacy simulations, these should be chosen so as to make the simulation span a round number of weeks. If not, the simulation span will be truncated: for instance, (1, 365) will be interpreted as (1, 364), i.e. 52 weeks (the last day of the last month will not be simulated). In Draft simulations, the simulation is always carried out on 8760 hours.

[^7]: changing the number of MC years will reset the playlist to its default value ; not available in Draft simulations

[^8]: Not available in Draft simulations

[^9]: KCG : Kirchhoff's constraints generator (see section 7)

[^10]: A typical case is given by the "Flow-Based" framework today implemented in a large portion of the European electricity market.
