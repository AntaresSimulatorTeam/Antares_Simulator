# Tests (developer)
This document describes the workflow used for the non-regression studies that are executed before every release. 
The goal of these tests is to avoid any unexpected change in the results, thus ensuring compatibility of the new 
versions with the old ones.

*Antares Simulator*'s non-regression tests are divided in two repositories:  
- [Antares_Simulator_Tests_NR](https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests_NR) contains the study batches (inputs). These batches are published in releases and do not contain any result.
- [SimTest](https://github.com/AntaresSimulatorTeam/SimTest) contains the study batches as well as reference results for some of the studies.

## Antares_Simulator_Tests_NR
This repository publishes releases. Each release contains the following files:

- `study-batches.txt`, a newline-separated list of study batches. A study batch is a group of Antares studies in the 
  same directory. Note that some studies may also be placed in a subdirectory.
- Archives containing the study batches.

Here is an example of one version of `study-batches.txt`:  
```
short-tests
long-tests-1
long-tests-2
long-tests-3
```

And here is the list of the corresponding archives:  
- `short-tests.zip`
- `long-tests-1.zip`
- `long-tests-2.zip`
- `long-tests-3.zip`

These archives can be found at the following URL: 
`https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests_NR/releases/download/<release tag>/<archive name>`

In each study, there is a `check-config.json` file containing the necessary information for running tests.

## SimTest
This is the source of releases. Each release contains the following files:
- `study-batches.txt`. It is merely a copy of `study-batches.txt` from Antares_Simulator_Tests_NR.
- For each study batch listed above, and for each target OS, an archive containing study results.

As above, `check-config.json` are packaged in the archives: one per study.

Here is a list of archives corresponding to the batches above (in this example, target OS are `windows-latest` and `ubuntu-latest`):

- `short-tests-windows-latest.zip`
- `long-tests-1-windows-latest.zip`
- `long-tests-2-windows-latest.zip`
- `long-tests-3-windows-latest.zip`
- `short-tests-ubuntu-latest.zip`
- `long-tests-1-ubuntu-latest.zip`
- `long-tests-2-ubuntu-latest.zip`
- `long-tests-3-ubuntu-latest.zip`

In the future, CentOS 7 should be included as well.

## Configuration files
Every study contains a `check-config.json` file that configures the test. Here is its content.

### Contents
[//]: # (TODO)
_**This section is under construction**_

#### name
The name of the test. Not really important, and currently not used.

#### checks
The list of checks that the test should conduct on the results.  
Contains one `output_compare` and one `integrity_compare` json objects.  

##### output-compare

##### integrity-compare

### Full example
```json

```

## Workflow
The workflow is the following:

1. Create a release in Antares_Simulator_Tests_NR (only if new studies are introduced). Tag it `X`.
2. Create a release in SimTest (only if a new feature is introduced, ie some results have changed) from Antares_Simulator_Tests_NR, tag `X` and Antares_Simulator, tag `Y`.
3. Use the release in SimTest to compare results of tag `Z` in Antares_Simulator. In this step, `pytest` is used as well as the `check-config.json` configuration files to test new results against the reference results.

Note that steps 1 and 2 are rarely used, therefore there is no need to automate them. 
Step 3, however, is to be run for every Antares_Simulator release, and so it [is automated in the 'run_tests' workflow](continuous-integration.md).
