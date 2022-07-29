# Study format changes
This is a list of all recent changes that came with new Antares Simulator features. The main goal of this document is to lower the costs of changing existing interfaces, both GUI and scripts.

## v8.4.0
### Input
    In section advanced parameters, file **settings/generaldata.ini**, in section `advanced-parameters`, add property `handle-negative-demand` [str]. Values can be

* `increase-effective-demand`
* `cap-net-demand`

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
