# File format changes

## v8.3.0
### Input
In file settings/generaldata.ini, add section `adequacy patch`, with keys

* `include-adq-patch` [bool]
* `set-to-null-ntc-from-physical-out-to-physical-in-for-first-step` [bool]
* `set-to-null-ntc-between-physical-out-for-first-step` [bool]

In existing section `optimization`, add key `include-split-exported-mps` [bool].

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
