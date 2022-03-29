# Tests (developer)
This document aims at describing the workflow used for the ~200 non-regression studies that are executed before every release. The goal of these tests is to avoid any unexpected change in the results, thus ensuring compatibility of the new versions with the old ones.

Note:
> This is a work-in-progress

## Repositories
There are two repositories :

- [Antares_Simulator_Tests_NR](https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests_NR) contains the study batches. These batches are published in releases and do not contain any result.
- [SimTest](https://github.com/AntaresSimulatorTeam/SimTest) contains the study batches as well as some reference results.

## Antares_Simulator_Tests_NR
This repository publishes releases. Each release contains the following files :

- `study-batches.txt`, a newline-separated list of study batches. A study batch is a group of Antares studies in the same directory. Note that some studies may also be placed in a sub-directory.
- Archives containing the study batches.

Here is an excerpt from the latest `study-batches.txt` 
```
short-tests
long-tests-1
long-tests-2
long-tests-3
```

Here is a list of the corresponding archives:

- `short-tests.zip`
- `long-tests-1.zip`
- `long-tests-2.zip`
- `long-tests-3.zip`

These archives can be found at the following URL: `https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests_NR/releases/download/<release tag>/<archive name>`

In each study, there is a config.json file containing the necessary information for running tests.

## SimTest
This is the source of releases. Each release contains the following files:
- `study-batches.txt`. It is merely a copy of `study-batches.txt` from Antares_Simulator_Tests_NR.
- For each study batch listed above, two archives containing results for Ubuntu and Windows.

As above, config.json are packaged in the archives: one per study.

Here is a list of archives corresponding to the batches above:

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
TODO

## Workflow
The workflow is the following:
1. Create a release in Antares_Simulator_Tests_NR (only if new studies are introduced). Tag it `X`.
2. Create a release in SimTest (only if a new feature is introduced) from Antares_Simulator_Tests_NR, tag `X` and Antares_Simulator, tag `Y`.
3. Use the release in SimTest to compare results of tag `Z` in Antares_Simulator. In this step, `pytest` is used as well as the `config.json` configuration files to test new results against the reference results.

Note that steps 1. and 2. are rarely used, therefore there is no need to automate them. Step 3. however is to be run for every Antares_Simulator release, as such it should be automated.
