# Antares Cucumber Tests

This module contains non-regression functional tests for Antares written in the [Gherkin language](https://cucumber.io/docs/gherkin/).  

## Tests structure

### Features, scenarios and tags
Features are supposed to represent big-picture features of the application. Every feature can have its own set of tests, 
defined in a `.feature` file. Features are under the [features](./features) folder.  
Every feature has multiple scenarios (every scenario represents a test case).  
A scenario can be tagged in order to add it to a category, allowing us to run the tests on a filtered subset of the 
scenarios later. The tags currently used in Antares are:

**Execution-time tags** (every scenario must carry exactly one):
- @short: simulation completes in under 30 seconds — run on all branches (feature/*, fix/*, develop push, scheduled)
- @medium: simulation completes in 30–120 seconds — run on develop push and scheduled only
- @long: simulation takes over 120 seconds, or is otherwise unfit for commit-time CI — run on scheduled only

**Orthogonal tags** (can be combined freely with the time tags above):
- @flaky: quarantine for tests that sometimes pass and sometimes fail — skipped by the CI by default (via `~@flaky`)
- @valid-hydro: scenarios from the valid-hydro study set
- @hydro, @storage: domain tags for hydro / storage scenarios
- @useless: legacy GUI training scenarios kept for historical reference
- @incomplete: scenario body is not yet fully written — kept to avoid losing the study reference

### Steps structure
Currently, tests are being migrated from the [legacy non-regression testing process](../run-study-tests). Thus, they 
all begin by defining the path to the study to run and then call antares, through the following "steps":
~~~gherkin
Given the solver study path is "someFolder/someStudy"
When I run antares simulator
~~~
The test will load the study, run antares-simulator, and hold on to its outputs.  
Next, assertion "steps" can be added. For example, this next assertion checks that the simulation time (as measured by 
antares-simulator and reported in its logs) is less than 15 seconds:
~~~gherkin
Then the simulation takes less than 15 seconds
~~~
And the next step checks the expected value of the annual system cost:
~~~gherkin
Then the expected value of the annual system cost is 3.725e+10
~~~

## Running the tests
### On your PC
First, you need to build antares-simulator. The tests will run the last antares-simulator executable built by the Cmake 
projet.  
Then, if needed, install the requirements by running:
~~~bash
pip install -r requirements.txt
~~~
Then just run the following to execute the tests:
~~~bash
cd src/tests/cucumber
behave
~~~
If you want to filter on a feature file and given tags, you can use:
~~~bash
behave --tags @some-tag features/some_feature.feature
~~~
Refer to the [behave documentation](https://behave.readthedocs.io/en/latest/) for more information.

### The configuration file
When you build antares-simulator, a file called `behave.ini` is automatically generated. This file contains the full 
path to the built antares executables, as well as the test resources.  
This file is used by behave to configure the tests: you may as well create it yourself. Here is the expected content:
~~~ini
[behave.userdata]
antares-solver = /path/to/antares-solver
antares-modeler = /path/to/antares-modeler
resources-path = /path/to/resources
linear-solver = coin
quadratic-solver = sirius
~~~
With:
- antares-solver (mandatory): the full path to the antares-solver executable you want to test (can be downloaded)
- antares-modeler (mandatory): the full path to the antares-modeler executable you want to test (can be downloaded)
- resources-path (mandatory): the full path to the test resources root
- linear-solver (optional): the solver you want to use during tests, for linear optimizations
- quadratic-solver (optional): the solver you want to use during tests, for quadratic optimizations

Alternatively, you can pass these options to behave through the command line. Beware that command-line options take 
precedence over options in the `behave.ini`file if both are defined. Example: 
~~~bash
behave -D antares-solver=/path/to/some/other/anatres-solver -D linear-solver=scip
~~~


### In the CI
Cucumber tests are run in the same way as the legacy tests in the Ubuntu & Windows CIs, except that they don't need the 
reference values from the SimTest repository, since reference values are stored explicitly in the feature files.
The CI runs three tiers of cucumber tests depending on the trigger:

| Tier | Tag filter | Trigger |
|------|-----------|---------|
| Short | `@short` | Every push (all branches) |
| Medium | `@medium` | Push to `develop` only |
| Long | `@long` | Scheduled nightly run only |

Tests marked `@flaky` are always skipped (via `~@flaky`).  
Workflow file: [here](../../../.github/workflows/cucumber-tests/action.yml)

## Under the hood
### Test files
Tests are hosted in the [Antares_Simulator_Tests_NR submodule](https://github.com/AntaresSimulatorTeam/Antares_Simulator_Tests_NR) 
into the `resources` folder. Adding or modifying a study should thus change contents of this submodule.

### Code-behind
All Gherkin steps have a code-behind definition called "step definitions". These are defined in the python files under 
[features/steps](./features/steps) and use the [behave](https://behave.readthedocs.io/en/latest/) implementation of 
cucumber. Feel free to add extra steps for your tests.
