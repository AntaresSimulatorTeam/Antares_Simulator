# Study format changes
This is a list of all recent changes that came with new Antares Simulator features. The main goal of this document is to lower the costs of changing existing interfaces, both GUI and scripts.

## v8.3.0
### Input
In file settings/generaldata.ini, add section `adequacy patch`, with keys

* `include-adq-patch` [bool]. Default value = `false`.
* `set-to-null-ntc-from-physical-out-to-physical-in-for-first-step` [bool]. Default value = `true`.
* `set-to-null-ntc-between-physical-out-for-first-step` [bool]. Default value = `true`

In existing section `optimization`, add key `include-split-exported-mps` [bool]. Default value = `false`

Add variables `DENS` and `Profit by plant`, which may be used for thematic trimming, see file settings/generaldata.ini, section `variables selection`.

### Output
* If `include-adq-patch` is set to `true`, add column `DENS` in files values-period.txt
* Add `Profit by plant` column in files details-period.txt
* If `include-split-exported-mps` is set to `true`, create splitted MPS files in the output folder
* Add file time_measurement.txt, containing performance data

NOTE : period can be any of the following

* `hourly`
* `daily`
* `weekly`
* `monthly`
* `annual`

## v8.2.0
### Input
* For each link, the first two columns of file input/links/area 1/area 2.txt have been moved to input/links/area 1/capacities/area 2_direct.txt and input/links/area 1/capacities/area 2_indirect.txt respectively. Note that these two files may contain more than one column, but must contain the same number of columns. The 6 remaining columns have been moved to area input/links/area 1/area 2_parameters.txt.

* In file settings/generaldata.ini, add `hydro-debug` [bool] key to existing section `output`. Default value is `false`.
