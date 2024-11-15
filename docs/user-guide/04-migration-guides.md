# Migration guides
This is a list of all recent changes that came with new Antares Simulator features. The main goal of this document is to lower the costs of changing existing interfaces, both GUI and scripts.

## v9.2.0
### Input
#### Removed properties
The following properties were removed from **settings/generaldata.ini**.
- `adequacy patch/enable-first-step`
- `adequacy patch/set-to-null-ntc-between-physical-out-for-first-step`
- `other preferences/initial-reservoir-levels`
#### Short-term storages
- Added property `efficiencywithdrawal` (double in [0, 1], default 1) short-term storages (file input/st-storage/clusters/<area id>/list.ini)
- Added timeseries cost-injection.txt, cost-withdrawal.txt and cost-level.txt. These files are optional. If present, they must contain either no value (same behavior as no file), or HOURS_PER_YEAR = 8760 coefficients in one column. Each of these timeseries is located in directory input/st-storage/series/<area id>/<ST id>/, along existing series (rule-curves.txt, etc.).

#### Final levels / scenario-builder
- Added optional key type "hfl" (hydro final level) in the scenario builder. The syntax is equivalent to existing prefix "hl" (hydro initial level), that is
```
hl,area,<year> = <value>
```

By convention, `year` start at 0 and `value` must be in interval [0, 1].

#### Compatibility flag for hydro pmax coefficients
In file settings/generaldata.ini, in section `other preferences`, add property `hydro-pmax-format` with possible values `daily` (default, legacy) and `hourly` (new).

Note: This flag allows to bypass the breaking change that took place in version 9.1 for hydro max powers. It is possible to support only the `legacy` file format. 


### (TS-generator only) TS generation for link capacities
In files input/links/<link1>/properties.ini, add the following properties
- unitcount (unsigned int, default 1)
- nominalcapacity (float, default 0)
- law.planned (string "uniform"/"geometric", default "uniform")
- law.forced (same)
- volatility.planned (double in [0,1], default 0)
- volatility.forced (same)
- force-no-generation (true/false, default true)

- "prepro" timeseries => input/links/<link 1>/prepro/<link 2>_{direct, indirect}.txt, 365x6 values, respectively "forced outage duration", "planned outage duration", "forced outage rate", "planned outage rate", "minimum of groups in maintenance", "maximum of groups in maintenance".
- "modulation" timeseries => input/links/<link 1>/prepro/<link 2>_mod_{direct, indirect}.txt, 8760x1 values each in [0, 1]
- number of TS to generate => generaldata.ini/General/nbtimeserieslinks (unsigned int, default value 1)


### Input
#### Hydro Final Reservoir Level
In the existing file **settings/scenariobuilder.dat**, under **&lt;ruleset&gt;** section following properties added (if final reservoir level specified, different from `init`):
* **hfl,&lt;area&gt;,&lt;year&gt; = &lt;hfl-value&gt;**

### Output
- Remove column SPIL ENRG CSR (adequacy patch)
- Add DTG MRG CSR and UNSP ENRG CSR variables

## v9.1.0
### Input 
#### Hydro Maximum Generation/Pumping Power
* For time series ![Migration diagram](img/migration.png "Migration diagram"), for more details, see [this Python script](img/migration.py)

Regarding Hydro time-series, the scenario builder allows the user to choose, for a given year and area, a different time series whether we consider :
- inflows, ROR and minimum generation, max pumping & generation (prefix "h")
- initial level (prefix "hl")
This implies that, inside one of the previous categories, the number of available time series is the same

![Logic changes](img/logic-hydro-maxP.png "Logic changes")

#### Short term storage groups
STS groups in input are now "dynamic" : group names are no longer fixed by code, user is free to define these groups.

In the "thematic trimming" section, the new dynamic variable `STS by group` is now used to enable/disable all variables (level/injection/withdrawal) for all groups. The following variables are obsolete and must not be provided

```
PSP_open_injection
PSP_open_withdrawal
PSP_open_level
...

Other1_injection
Other1_withdral
Other1_level
...
Other5_injection
Other5_withdral
Other5_level
```
(3*9=27 variables in total)

The default value for group is "OTHER1".

### Output
#### ST Storage

- Output columns for ST storage are capitalized. For any STS group name my_group, 3 output columns are created : `MY_GROUP_INJECTION`, `MY_GROUP_WITHDRAWAL`, `MY_GROUP_LEVEL`.
- If a group is empty, no column is produced.
- There is now a variable number of columns in files values-XXX.txt, depending on the groups of ST storages provided by the user. Note that groups are case-insensitive, for example `battery`, `Battery` and `BATTERY` all represent the same group. If no ST storage exist for a given area, no variables associated to ST storages will be produced.

## v9.0.0
### Input
#### Study version
Breaking change in the study format, file **study.antares**
```
version = 900
```
becomes
```
version = 9.0
```
Compatibility is kept with versions up to 8.8.0. Starting from version 9.0.0, the new format must be used.


## v8.8.0
### Input
#### Short-term storage
If no value is specified for `initiallevel`, then a default value of 50% is used. Note that this value is used only if `initialleveloptim=false`, and that `false` is the default value for `initialleveloptim`.

Add property `enabled` (bool, default=`true`). If a ST storage object is not enabled, it is ignored by Antares Simulator.

#### Solver logs (expert option)
Save solver logs by setting `solver-logs = true` (default : `false`) under the `optimization` section.

#### Experimental "MILP" mode
New value `milp` for existing property `other preferences/unit-commitment-mode` in file **settings/generaldata.ini**.

Using this property requires OR-Tools and a MILP solver (XPRESS, COIN)

```
antares-8.8-solver --use-ortools --ortools-solver coin|xpress ...
```

### Output
### Cashflow by short-term storage
In existing file **details-STstorage-&lt;period&gt;.txt** (mc-all & mc-ind), add a new column for each ST storage object, named "STS Cashflow By Cluster", in EURO.

## v8.7.0
### Input
#### Scenarized RHS for binding constraints
- For each binding constraint, file **input/bindingconstraints/&lt;id&gt;.txt** is split into 3 files:
    - **input/bindingconstraints/&lt;id&gt;_lt.txt**
    - **input/bindingconstraints/&lt;id&gt;_gt.txt**
    - **input/bindingconstraints/&lt;id&gt;_eq.txt**

    Each of these files can be either empty or have N column of 8674 rows

- In file **input/bindingconstraints/bindingconstraints.ini**, add property `group` to every section
- Binding constraints in the same group must have the same number of RHS columns (3 files described above for their respective types). Exception: a constraint in a group can have empty RHS or only one RHS column
- In file **settings/scenariobuilder.dat**, add prefix `bc` for every group of binding constraints. The syntax is the following
```
bc,<group>,<MC Year> = <TS number>
```
This line is not mandatory for every group & MC year. If absent, the TS number will be drawn randomly (usual behavior).

- 0 &lt;= MC Year &lt; generaldata.ini/general.nbyears
- 1 &lt;=TS number &lt;= number of columns for the group

#### Thermal cluster new properties
For each thermal cluster, in existing file **input/thermal/clusters/&lt;area&gt;/list.ini**, under existing sections **&lt;cluster&gt;**, following properties added: 

* `costgeneration` [string] can take values `useCostTimeseries` or be excluded from the section if `SetManually` is selected (default behavior).
* `efficiency` [float] between 0-100 (default = 100). Unit is "percentage".
* `variableomcost` [float] excluded from the section if default value 0 is selected (default behavior). Unit is Euro / MWh

For each thermal cluster, new files added **input/thermal/series/&lt;area&gt;/&lt;cluster&gt;/CO2Cost.txt** and **input/thermal/series/&lt;area&gt;/&lt;cluster&gt;/fuelCost.txt**. **fuelCost.txt** and **CO2Cost.txt** must either have one column, or the same number of columns as existing file **series.txt** (availability). The number of rows for these new matrices is 8760.

### Output
#### Scenarized RHS for binding constraints
Add directory **bindingconstraints** to output directory **ts-numbers**. For every binding constraint group, add a file **ts-numbers/bindingconstraints/&lt;group&gt;.txt** containing the TS numbers used for that group.

## v8.6.0
### Input
#### Short-term storage
* Add directories **input/st-storage/clusters** and **input/st-storage/series**
* For each area, add directory **input/st-storage/clusters/&lt;area id&gt;/list.ini**
* This file contains the multiple sections whose name is ignored. Each section contains these properties:
    * `name` [str]
    * `group` [str]. Possible values: "PSP_open", "PSP_closed", "Pondage", "Battery", "Other1", ... , "Other5". Default Other1
    * `efficiency` [double] in range 0-1
    * `reservoircapacity` [double] &gt; 0
    * `initiallevel` [double] in range 0-1
    * `withdrawalnominalcapacity` [double] &gt; 0
    * `injectionnominalcapacity` [double] &gt; 0
    * `initialleveloptim` [bool]

* For each short-term-storage object, add the corresponding time-series in directory **input/st-storage/series/&lt;area id&gt;/&lt;STS id&gt;**. All of these files contain 8760 rows and 1 column.
    * **PMAX-injection.txt** All entries must be in range 0-1
    * **PMAX-withdrawal.txt** All entries must be in range 0-1
    * **inflows.txt** All entries must be &gt; 0
    * **lower-rule-curve.txt** All entries must be in range 0-1
    * **upper-rule-curve.txt** All entries must be in range 0-1

#### Pollutant emission factors
In files **input/thermal/cluster/area/list.ini** add properties `nh3`, `nox`, `pm2_5`, `pm5`, `pm10`, `nmvoc`, `so2`, `op1`, `op2`, `op3`, `op4`, `op5` [double]. These properties are emission factors similar to the existing one for CO2.

#### Adequacy patch
In file **settings/generaldata.ini**, in section `adequacy patch` add property `enable-first-step` [bool]. Default value = `true` Enable or disable DENS column

#### Hydro Pmin
For each area, new file added **input/hydro/series/&lt;area&gt;/mingen.txt**. This file has one or more columns, and 8760 rows. The number of columns may be 1, or identical to the number of columns in existing file **mod.txt**.

### Output
#### Short-term storage
* For every short-term storage group, add 3 columns in files **values-&lt;period&gt;.txt** (mc-all & mc-ind)
    * `ST-<group id>-withdrawal`
    * `ST-<group id>-injection`
    * `ST-<group id>-level`
* For every area, add file **details-STstorage-&lt;period&gt;.txt** (mc-all & mc-ind) containing the same columns, but this time for every short-term storage object.

#### Pollutant emission factors
In files **economy/mc-all/areas/** add column: CO2 EMIS. One colum for every pollutant: CO2, NH3, NOX, PM2\_5, PM5, PM10, NMVOC, OP1, OP2, OP3, OP4, OP5

## v8.5.2
### Input
In file **settings/generaldata.ini**, in section `optimization`, link-type is now deprecated

## v8.5.0
### Input
In file **settings/generaldata.ini**, in section `adequacy patch`, add properties

* `price-taking-order` [string] can take values `DENS` (default value) and `Load`.
* `include-hurdle-cost-csr` [bool]. Default value = `false`
* `check-csr-cost-function` [bool]. Default value = `false`
* `threshold-initiate-curtailment-sharing-rule` [double]. Default value = `0.0`
* `threshold-display-local-matching-rule-violations` [double]. Default value = `0.0`
* `threshold-csr-variable-bounds-relaxation` [int]. Default value = `3`

### Output
* If `include-adq-patch` is set to `true`, add column `LMR VIOL.` in files **values-&lt;period&gt;.txt** (mc-all & mc-ind)
* If `include-adq-patch` is set to `true`, add column `SPIL. ENRG. CSR` in files **values-&lt;period&gt;.txt** (mc-all & mc-ind)
* If `include-adq-patch` is set to `true`, add column `DTG MRG CSR` in files **values-&lt;period&gt;.txt** (mc-all & mc-ind)

## v8.4.0
### Input
#### Zero/infinite capacity for physical links only
In file **settings/generaldata.ini**, in section `optimization`, change admissible values for key `transmission-capacities` [str]:

* `local-values` (formerly `true`, default) uses the local capacity of all links
* `null-for-all-links` (formerly `false`) sets the capacity of all links to 0
* `infinite-for-all-links` (formerly `infinite`) sets the capacity of all links to +infinity
* `null-for-physical-links` sets the capacity of physical links to 0, uses the local capacity for virtual links
* `infinite-for-physical-links` sets the capacity of physical links to +infinity, uses the local capacity for virtual links

Previous values (`true`, `false` and `infinite`) are still admissible for compatibility.

### Remove "Split MPS"
In existing section `optimization`, remove property `include-split-exported-mps` [bool]. This property will be ignored (no error) for compatibility.

#### Result format
In file **settings/generaldata.ini**, in existing section `output`, add property `result-format` [str]. Default value = `txt-files`. If this property is set to `zip`, all results are written into a single zip archive, instead of multiple files.

### Output
#### Result format
If property `output/result-format` is set to `zip`, all results are stored in a single archive. The hierarchy within this archive remains identical, for example **economy/mc-all/areas/**. Otherwise, txt files are created like in previous versions.

## v8.3.2
### Writing MPS files
MPS files of first optimization used to be overwritten by MPS files of second optimization. Not anymore.
Now user can choose to print :
* no MPS file,
* MPS files related to the first optimization (named **problem-&lt;week&gt;-&lt;year&gt;--optim-nb-1.txt**),
* MPS files related to the second optimization (named **problem-&lt;week&gt;-&lt;year&gt;--optim-nb-2.txt**),
* MPS files related to the both optimizations.

In the **generaldata.ini** input file, corresponding values for **include-exportmps** are : **none**, **optim-1**, **optim-2**, **both-optims**.

Compatibility with existing values is maintained (**true** = **both-optims**, **false**=**none**).

### Marginal price of a binding constraint
#### Input
In the context of the addition of a new output variable (marginal price associated to a binding constraint), file **input/bindingconstraints/bindingconstraints.ini** get 2 new parameters for each binding constraint.
They control which marginal price time granularity is printed, either regarding year by year or synthesis results.

* `filter-year-by-year`. Default value = hourly, daily, weekly, monthly, annual
* `filter-synthesis`. Default value = hourly, daily, weekly, monthly, annual

#### Marginal cost for binding constraints
Still on the binding constraints marginal price results, 2 new folders **binding_constraints** are created inside any simulation output folder, more precisely under **mc-ind** and **mc-all**.

Examples :
* **output/yyyymmdd-hhmmeco/economy/mc-ind/00001/binding_constraints**
* **output/yyyymmdd-hhmmeco/economy/mc-all/binding_constraints**

These folders are meant to contain results of any kind regarding binding constraints marginal price (year by year or synthesis).

Examples of output files inside these folders :
* **binding-constraints-hourly.txt**
* **binding-constraints-weekly.txt**

## v8.3.1
### Output
Add file **execution_info.ini**, containing information about the execution time for the various steps of a study, as well as study related information that affects performance.

```ini
[durations_ms]
hydro_ventilation = 7
mc_years = 693
post_processing = 0
study_loading = 51
synthesis_export = 731
total = 1526
yby_export = 114

[number_of_calls]
hydro_ventilation = 1
mc_years = 1
post_processing = 1
study_loading = 1
synthesis_export = 1
total = 1
yby_export = 1

[optimization problem]
constraints = 1008
non-zero coefficients = 15894
variables = 1512

[study]
antares version = 831
areas = 3
enabled bc = 0
enabled daily bc = 0
enabled hourly bc = 0
enabled thermal clusters = 1
enabled weekly bc = 0
links = 2
max parallel years = 1
ortools solver = xpress
ortools used = true
performed years = 1
unit commitment = fast
```

## v8.3.0
### Input
In file **settings/generaldata.ini**, add section `adequacy patch`, with properties

* `include-adq-patch` [bool]. Default value = `false`
* `set-to-null-ntc-from-physical-out-to-physical-in-for-first-step` [bool]. Default value = `true`
* `set-to-null-ntc-between-physical-out-for-first-step` [bool]. Default value = `true`

In existing section `optimization`, add property `include-split-exported-mps` [bool]. Default value = `false`

Add variables `DENS` and `Profit by plant`, which may be used for thematic trimming, see file **settings/generaldata.ini**, section `variables selection`.

For each area, add a new file **input/areas/&lt;area&gt;/adequacy_patch.ini** containing a single section `adequacy-patch`. This section contains a single property `adequacy-patch-mode`, that can take values

* `outside`
* `inside`
* `virtual`

Example
```
[adequacy-patch]
adequacy-patch-mode = outside
```

### Output
* If `include-adq-patch` is set to `true`, add column `DENS` in files **values-&lt;period&gt;.txt** (mc-all & mc-ind)
* Add `Profit by plant` column in files **details-&lt;period&gt;.txt** (mc-all & mc-ind)
* If `include-split-exported-mps` is set to `true`, create split MPS files in the output folder
* Add file **time_measurement.txt**, containing performance data

NOTE: **period** can be any of the following

* `hourly`
* `daily`
* `weekly`
* `monthly`
* `annual`

## v8.2.0
### Input
* For each link, the first two columns of file **input/links/&lt;area 1&gt;/&lt;area 2&gt;.txt** have been moved to **input/links/&lt;area 1&gt;/capacities/&lt;area 2&gt;_direct.txt** and **input/links/&lt;area 1&gt;/capacities/&lt;area 2&gt;_indirect.txt** respectively. Note that these two files may contain more than one column, but must contain the same number of columns. The 6 remaining columns have been moved to area **input/links/&lt;area 1&gt;/&lt;area 2&gt;_parameters.txt**.

* In file **settings/generaldata.ini**, add `hydro-debug` [bool] key to existing section `output`. Default value is `false`.

### Output
If parameter `hydro-debug` is enabled, a **debug** directory is created in the output folder.
