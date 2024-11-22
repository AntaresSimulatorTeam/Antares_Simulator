# Parameters

_**This section is under construction**_

[//]: # (TODO: check that these parameters are not used in the solver.)
[//]: # (If they are used by the solver -but only in case some TS are generated-, keep them here but add cross mentions to them)

---
## General parameters
These parameters are listed under the `[general]` section in the `.ini` file.

---
### Time-series parameters


#### generate
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `max-power` (ex: `generate = load, hydro, wind`)
- **Required:** no
- **Default value:** empty
- **Usage:** if left empty, all the time-series will be imported from input files. Else, the time-series
  listed in this parameter will be generated randomly by the simulator.

> _**WARNING:**_ time-series generation is not available for transmission capacities (NTC)

---
#### nbtimeseriesload
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `load` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

---
#### nbtimeserieshydro
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `hydro` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

---
#### nbtimeserieswind
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `wind` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

---
#### nbtimeseriesthermal
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `thermal` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

---
#### nbtimeseriessolar
- **Expected value:** unsigned integer
- **Required:** no
- **Default value:** 1
- **Usage:** if `solar` time-series are [automatically generated](#generate), this parameter sets the number of different
  time-series to generate.

---
#### refreshtimeseries
[//]: # (TODO: verify usage)
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that have
  to be periodically renewed in run-time

> _**WARNING:**_ time-series refresh is not available for transmission capacities (NTC)

---
#### intra-modal
[//]: # (TODO: verify usage)
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that are
  correlated, i.e. for which the same time-series should be used together in a given scenario.

> _**WARNING:**_ inter-modal correlation is not available for transmission capacities (NTC)  
> _**WARNING:**_ this is the historical correlation mode

---
#### inter-modal
[//]: # (TODO: document this parameter)
_**This section is under construction**_  
- **Expected value:** 
- **Required:** **yes** 
- **Default value:** 
- **Usage:** 

> _**Note:**_
> A full meteorological correlation (for each MC year) is, from a theoretical standpoint, achievable by activating
> "intramodal" and "intermodal" for all but the `thermal` kind of time-series. The availability of an underlying
> comprehensive multidimensional meteorological database of ready-made time-series is the crux of the matter when it
> comes to using this configuration.

---
#### improveunitsstartup
[//]: # (TODO: document this parameter -seems deprecated-)
_**This section is under construction**_  
- **Expected value:** 
- **Required:** **yes** 
- **Default value:** 
- **Usage:**

---
#### refreshintervalload
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `load` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

---
#### refreshintervalhydro
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `hydro` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

---
#### refreshintervalwind
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `wind` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

---
#### refreshintervalthermal
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `thermal` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).

---
#### refreshintervalsolar
- **Expected value:** strictly positive integer
- **Required:** no
- **Default value:** 100
- **Usage:** if `solar` time-series are automatically [generated](#generate) and [refreshed](#refreshtimeseries), this
  parameter sets their refresh interval (in number of Monte-Carlo years).


---
## Input parameters
These parameters are listed under the `[input]` section in the `.ini` file.

---
#### import
- **Expected value:** comma-seperated list of 0 to N elements among the following (case-insensitive):
  `load`, `wind`, `hydro`, `thermal`, `solar`, `renewables`, `ntc`, `max-power`
- **Required:** no
- **Default value:** empty
- **Usage:** if some time-series are [automatically generated](#generate), this parameter selects those of them that
  should be exported into the input files (in replacement of the original ones).

> _**Note:**_
> you can also use [archives](../solver/static-modeler/04-parameters.md#archives) to store the time-series in the output folder.

---
## Seeds - Mersenne Twister parameters
These parameters are listed under the `[seeds - Mersenne Twister]` section in the `.ini` file.  
They allow the user to set the seeds of random number generators, in order to ensure the results are repeatable.

---
#### seed-tsgen-load
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `load` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

---
#### seed-tsgen-hydro
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `hydro` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

---
#### seed-tsgen-wind
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `wind` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

---
#### seed-tsgen-thermal
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `thermal` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.

---
#### seed-tsgen-solar
- **Expected value:** unsigned integer
- **Required:** **yes**
- **Usage:** if `solar` time-series are [automatically generated](#generate), this parameter fixes the seed for its
  random generator.