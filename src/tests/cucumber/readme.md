# Antares Cucumber Tests

This module contains non-regression functional tests for Antares written in the [Gherkin language](https://cucumber.io/docs/gherkin/).  

## Tests structure

### Features, scenarios and tags
Features are supposed to represent big-picture features of the application. Every feature can have its own set of tests, 
defined in a `.feature` file. Features are under the [features](./features) folder.  
Every feature has multiple scenarios (every scenario represents a test case).  
A scenario can be tagged in order to add it to a category, allowing us to run the tests on a filtered subset of the 
scenarios later. The tags currently used in Antares are:
- @fast: tests that run fast
- @slow: tests that run slow
- @short: tests from the legacy "short-tests" batch
- @medium: tests from the legacy "medium-tests" batch
- @flaky: quarantine for flaky tests (i.e. sometimes pass, sometimes fail) that are to be skipped by the CI

### Steps structure
Currently, tests are being migrated from the [legacy non-regression testing process](../run-study-tests). Thus, they 
all begin by defining the path to the study to run and then call antares, through the following "steps":
~~~gherkin
Given the study path is "someFolder/someStudy"
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

### In the CI
Cucumber tests are run in the same way as the legacy tests in the Ubuntu & Windows CIs, except that they don't need the 
reference values from the SimTest repository, since reference values are stored explicitly in the feature files.
Note that tests marked as "@flaky" are skipped by default.  
Workflow file: [here](../../../.github/workflows/cucumber-tests/action.yml)

## Under the hood
### Test files
Tests are hosted in the [simulator-cucumber-tests submodule](https://github.com/AntaresSimulatorTeam/simulator-cucumber-tests) into the `src/test/cucmber` folder. Adding or modifying a study should thus change contents of this submodule.

### Code-behind
All Gherkin steps have a code-behind definition called "step definitions". These are defined in the python files under [features/steps](./features/steps) and use the [behave](https://behave.readthedocs.io/en/latest/) implementation of cucumber. Feel free to add extra steps for your tests.
