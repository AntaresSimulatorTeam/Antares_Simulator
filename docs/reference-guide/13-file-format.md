# Study format changes
This is a list of all recent changes that came with new Antares Simulator features. The main goal of this document is to lower the costs of changing existing interfaces, both GUI and scripts.
## v8.8.0
For each area, new files are added **input/hydro/series/&lt;area&gt;/maxgen.txt** and **input/hydro/series/&lt;area&gt;/maxpump.txt**. These files have one or more columns, and 8760 rows. The number of columns in these two files must be the same, if there is more than one column in each file, but if there is just one column for example in maxgen.txt file, maxpump.txt file can have more than one column and vice versa.
### Input
Under `Configure/MC Scenario Builder` new section added `Hydro-Power-Credits`
In the existing file **settings/scenariobuilder.dat**, under **&lt;ruleset&gt;** section following properties added:
* **hgp,&lt;area&gt;,&lt;year&gt; = &lt;hgp-value&gt;**
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

* `costgeneration` [string] can take values `useCostTimeseries` or be excluded from the section if `Set manually` is selected (default behavior).
* `efficiency` [float] excluded from the section if default value 100 is selected (default behavior).
* `variableomcost` [float] excluded from the section if default value 0 is selected (default behavior).

For each thermal cluster, new files added **input/thermal/prepro/&lt;area&gt;/&lt;cluster&gt;/CO2Cost.txt** and **input/thermal/series/&lt;area&gt;/&lt;cluster&gt;/fuelCost.txt**. **fuelCost.txt** and **CO2Cost.txt** must either have one column, or the same number of columns as existing file **series.txt** (availability)

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
    * `group` [str]. Possible values: "PSP_open", "PSP_closed", "Pondage", "Battery", "Other_1", ... , "Other_5". Default Other_1
    * `efficiency` [double] in range 0-1
    * `reservoircapacity` [double] &gt; 0
    * `initiallevel` [double] in range 0-1
    * `withdrawalnominalcapacity` [double] &gt; 0
    * `injectionnominalcapacity` [double] &gt; 0
    * `initialleveloptim` [bool]

* For each short-term-storage object, add the corresponding time-series in directory **input/st-storage/series/&lt;area id&gt;/&lt;STS id&gt;**. All of these files contain 8760 rows and 1 column.
    * **PMAX-injection.txt** All entries must be in range 0-1
    * **PMAX-withdrawal.txt** All entries must be in range 0-1
    * **inflow.txt** All entries must be &gt; 0
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
