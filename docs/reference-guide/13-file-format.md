# Study format changes
This is a list of all recent changes that came with new Antares Simulator features. The main goal of this document is to lower the costs of changing existing interfaces, both GUI and scripts.

## v8.4.0
### Input
In file **settings/generaldata.ini**, in existing section `output`, add property `result-format` [str]. Default value = `txt-files`. If this property is set to `zip`, all results are written into a single zip archive, instead of multiple files.

### Output
#### Result format
If property `output/result-format` is set to `zip`, all results are stored in a single archive. The hierarchy within this archive remains identical, for example **economy/mc-all/areas/**. Otherwise, txt files are created like in previous versions.
In the context of the addition of a new output variable (marginal price associated to a binding constraint), file **input/bindingconstraints/bindingconstraints.ini** get 2 new parameters for each binding constraint.
They constrol which marginal price time granularity is printed, either regarding year by year or synthesis results.

* `filter-year-by-year`. Default value = hourly, daily, weekly, monthly, annual
* `filter-synthesis`. Default value = hourly, daily, weekly, monthly, annual

#### Marginal cost for binding constraints
Still on the binding constraints marginal price results, 2 new folders **binding_constraints** are created inside any simulation output folder, more precisely under **mc-ind** and **mc-all**.

Examples : 
* `output/yyyymmdd-hhmmeco/economy/mc-ind/00001/bind_const`
* `output/yyyymmdd-hhmmeco/economy/mc-all/bind_const`

These folders are meant to contain results of any kind regarding binding constraints marginal price (year by year or synthesis).

Examples of output files inside these folders :
* `binding-constraints-hourly.txt`
* `binding-constraints-weekly.txt`


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
* If `include-split-exported-mps` is set to `true`, create splitted MPS files in the output folder
* Add file **time_measurement.txt**, containing performance data

NOTE : **period** can be any of the following

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
