# How does the automatic testing script system work ?
## Introduction
Here is an automatic testing python script system.

This program performs the following : 
1. Searches for all studies in a given directory
2. From each study, code retrieves a check configuration, local to that study, in order to make checks on the simulation results of that study (see items below)
3. Runs a simulation on each study, given the local configuration.
4. Given the results of the simulation on a study, makes the checks retrieved at step 2 on these results.
   Examples :
    - make sure current results and reference resuts are identical
    - check for existence or even content of output files

Each study used for testing contains (at its root folder) a file **check-config.json** (build manually) that roughy contains : 
- a name,
- a executable to run the study with,
- a list of options to add to running command,
- a set of checks to be done after run, on the results of that run.

If a study contains no **check-config.json**, it's skipped and logged.

## study configuration (check-config.json)

Each `check-config.json` is actually an array. 
Each entry describes one run, and must have a `name`, an `exe`, an `options` array, and `checks`.

```json
[
    {
        "name": "outputs_1node_ltstorage",
        "exe": "antares-solver",
        "options": [
            "--solver=sirius",
            "--output=simulation-tables",
            "--simulation-table-stages=optim-nb-1"
        ],
        "checks": {
            "simu_table_compare": null
        }
    }
]
```

`exe` is either `antares-solver` or `antares-problem-generator` for now, not a binary path. 
`options` contains literal command-line arguments in order. 
Use `[]` for no extra options. Flags, repeated options, and values containing spaces are
supported; do not add shell quoting around individual arguments.

The runner supplies the study argument automatically: `-i <study>` for
`antares-solver`, or a positional `<study>` for `antares-problem-generator`.

To run a study in multiple ways, add to this .yml file separate named entries,
with the corresponding checks and options.

## Entry points to the automatic testing script system

For now, there are 2 entry points that can be used to run tests automatically.
- test_from_json.py
- test_unfeasible_problem.py

The first one runs tests as we explained above, the second one is a bit different : it does not search for studies, but is given an explicit path to a study.
And this study does not contain any check config definition under the form of a json file : run and checks are explicitly defined and supplied in the tests.

We won't comment the second script, as understanding the first one should ease understanding the second.

<span style="color:blue">TO DO : maybe we should think of integrating tests on unfeasible problems in the regular testing system ? But it would make associated check-config.json more complicated, as well as the json schema.</span>

## File test_from_json.py
Here we examine the second part of script **test_from_json.py**. Note that The first part is dedicated to finding studies and storing the definition of the checks to be made on results after a simulation is run on studies (recall the **check-config.json** file).

```python
1 @pytest.mark.json
2 @pytest.mark.parametrize('study_path, json_config', json_collector.pairs(), ids=json_collector.testIds())
3 def test(study_path, json_config, check_runner, exe_paths):
4     simulation = create_simulation(study_path, json_config, exe_paths)
5     checks = create_checks(study_path, json_config["checks"], simulation=simulation)
6     check_runner.run(simulation, checks)
```

The content of this script is quite short, despite all the tests it has to perform. This is partially due to the power of **pytest** and its features : **fixtures** and especially **parametrization**, which is responsible for running a test many times, each time with a different set of arguments.

In the following, we comment the content of this script. Lines of this scripts are numbered so that they can be refered to whenever we need.

**Line 1** : Following tests are marked as a collection, that is belonging to the same category

## Fixtures in pytest (generalites)
**pytest** comes with the notion of **fixture**. 
Fixtures allow executing a piece of code just before a test runs.
To take benefit of a fixture, a test needs to be given this fixture as argument. 
Fixture themselves can also be given arguments, we'll see how we do it (in the context of the current testing system) when we talk about **parametrization**.
Fixtures return a result to be used in the test.
Let's look at a simple test :
```python
@pytest.fixture
def my_fixture():
    # Define the result some_result to be returned (can be a variable or constant of any kind)
    return some_result

def my_test(my_fixture):
    # Some use of "my_fixture", as the result of the fixture "my_fixture" itself.
    # Some checks.
```
When **my_test** executes, it first calls **my_fixture**, which returns a result.
This result can be anything. So it can be a constant (like a string) or an object.

Note that, when a test takes a fixture as argument, this argument is both a way to call the fixture and represents the result of the fixture itself.   
As a result, the argument can be (and should be) used in the test.
We'll see examples of this in the following.

A fixture can be a bit more complex than the previously displayed one : it can be divided in 2 parts.
The first would be a **setup** operation (executed just before the test begins) and the second part would be a **teardown** operation (executed just before the test ends).
In the previous example, the fixture only has a **setup** part.

In order to supply a fixture with both **setup** and **teardown**, we need to use the **yield** python keyword. The **yield** instruction returns the fixture's result back to the test.

```python
@pytest.fixture
def my_fixture():
    obj  = some_class("""some args""")
    yield obj # Returns obj before the test executes
    # Here some teardown instruction, therefore performed just before the test's execution ends.

def my_test(my_fixture):
    # Some use of "my_fixture", as the result of the fixture "my_fixture" itself.
    # Some checks.
```

Fixtures can be supplied with parameters. This can be done by giving an argument to the fixture.
In the following snippet, the fixture **study_path** only returns the parameter that it's given.
```python
@pytest.fixture()
def study_path(request):
    return request.param
```
We'll see at least another example of such feature later on. 

Another trait of fixtures is that they can be nested : a fixture can call other fixtures.
**fixture 1** can have a **fixture 2** as an argument. This means that, when **fixture 1** comes to execute, **fixture 2** is called before execiting **fixture 1**'s body, and so **fixture 1** has access to **fixture 2**'s results during its execution.

## Tests parametrization
Back to script **test_from_json.py**.

Let's recall **line 3** :
```python
3 @pytest.mark.parametrize('study_path, json_config', json_collector.pairs(), ids=json_collector.testIds())
```

**@pytest.mark.parametrize** decorator takes **json_collector.pairs()** as argument.
The latter returns a **list** of pairs : 
- first element (of a pair) is a path of a study
- second argument contains the data retrieved from the json file associated to the study.   

For each pair (or each study) of this list, each element of the pair is stored in parameterization arguments : (<span style="color:red">'study_path, json_config'</span>), and the associated test is given access to these 2 parameters. 

It's worth to say : 
- **json_config** is used directly as a parameter of the test : a test directly access the json configuration of a study through this parameter
- **study_path** parameter is used in an indirect way. Actually, **study_path** parameter is used through a fixture named **study_path** that retrieves the value of the parameter.


## The test's body
With the previous explanations in mind, we're can describe what's happening when tests are run.

So the test is run multiple times due to **parametrization** : the test body is executed as many times as there are elements is the list **json_collector.pairs()**, that is as many studies spotted by the script **test_from_json.py** (see first part of the script, not commented in this doc).

### <ins>Fixtures in this scripts</ins>
File **conftests.py** registers all fixtures that are used at each test run : 
```py
pytest_plugins = ["registered_fixures"]
```
This code means that all fixtures run before each test can be found in a python named **registered_fixures.py**.

Here we talk about **line 3** of script **test_from_json.py**.
```python
def test(study_path, json_config, check_runner, exe_paths):
    ...
```
For a given run of the test's body, that is for each study previously found in a directory, some fixtures are first run.
1. **study_path** : as already said above, this fixture is supplied with the path of the current study as only argument. Note that this fixture just returns this path, so this path is available for any use in the current test.
2. **json_config** : is not a fixture, it's a parameter coming from parameterization. See above.
3. **check_runner** : it's a fixture, defined in **registered_fixures.py**.

```python
@pytest.fixture(autouse=True)
def check_runner(simulation, resultsRemover):
    # Actions done before the current test
    my_check_handler = check_handler(resultsRemover)
    try:
        # Running the current test here
        yield my_check_handler
    finally:
        # Teardown : actions done after the current test
        my_check_handler.teardown()
```
As we can see, this fixture is a **setup/teardown** fixture, and it calls  another fixtures (**resultsRemover**) before running.
So when **check_runner** runs :
- A remove of resuls associated to this simulation is prepared (but not run yet)
- A **check handler** object is returned to the current test : it is made accessible to the test through the tets argument **check_runner**. We'll see that this **check handler** is responsible for cleaning the simulation results after a test is completed.

Note that the last line here is executed when the current test ends.

4. **exe_paths** : this fixture is executed once (see its definition and especially its **scope**) per session, and give access to the paths of possible executables run on studies in a test.
Note that this paths are retrieved from a **executables.ini** file generated at cmake config step.

### <ins>simulation run factory</ins>
```py
    simulation = create_simulation(study_path, json_config, exe_paths)
```
The returned variable is the simulation run we want to make : in order to do that, we need all 3 arguments.
- **study_path** : path to current study. This is the study path to run the simulation uppon.
- **json_config** supplies which executable we want to run the study with, as well as the options to be added to the run command. 
- **exe_paths** gives access to the paths of the executable chosen in the test configuration. 

### <ins>Checks creation factory</ins>
This concerns **line 5** of script **test_from_json.py**.
```python
    checks = create_checks(study_path, json_config["checks"], simulation=simulation)
```
Here we create a list of checks to perform on the results of a simulation on the current study is created.

This list is built from the description of the checks contained in **json_config**.

For that, **create_checks** is called, with natural information for a check : the path to the study, used to fetch results to be checked after run, and the kind of things to check (in **json_config**). The left argument of **create_checks** is the simulation previoulsy prepared by fixture **check_runner**. This simulation is needed for special kinds of check : the ones that need the return code of Antares Simulator just after it has run.

Note that all checks in that list is an instance of a class necessarily derived from the more general class **check_interface**. 
This parent class forced every child to have a method **run()** and a method **name()**, so that, when this list is traversed for any reason, a call to the **run()** or **name()** method does not fail.

Note also that, at this stage, the simulation has not been run yet, but will be run at the next line of **test_from_json.py**.

### <ins>Running the checks</ins>
This is about **line 6** of script **test_from_json.py**.
```python
    check_runner.run(simulation, checks)
```
This is the place where the script actaully runs the checks created in the previous code line (see above).

### <ins>End of tests</ins>
**check_runner** fixture is a **setup/teardown** fixture. This means that when each test associated to a study ends, the **teardown** part of this fixture is run.

Looking at the content of **check_handler.teardown()** method, we see what it does :
- It puts back the study's state where it first was. Indeed, some checks modify the studies for their own purpose. The simulation is run with these new study parameters and checks are performed taking into account these changes. If several checks are made on the same study's results, and if every check requires a new simulation run, a tests can fail because a previous one made a change on the study that the current check does not expect. To avoid that, these changes are removed.
- It removes the simulation results. Again, if several checks are made on a study, that may imply as much associated simulation runs, and in these conditions, retrieving some results' data in a study that has several simulation results directories can lead to an unexpected failure in tests.

# How to add a check on a test ?
We explain how to define a new kind of check on a study, that is a non existing check.
This implies we need to add code for this new check.
## What to do ?
1. Create a new python file in the **src/tests/run-study-tests/check_on_results**
2. In this file, add the definition of a new class associated to the new check on a simulation run. This class must be derived from the parent class **check_interface**. This parent class forces you to define a **run()** and a **name()** method to your check class.
3. You'll probably want to add your new check to one or more studies. In this case, you need to add your check to the associated **check-config.json**. This implies :
   - Defining a **json** representation of your check and how to insert it in a **check-config.json**
   - Adding these definitions in a **json schema**. Note that every found **check-config.json** is verified against the **json schema**
The schema can be found at : **src/tests/run-study-tests/parse_studies/json_schema.json**
4. Add the new check in the **check-config.json** files you need to.

# Running the script

```bash
> cd src/tests/run-study-tests
> python -m pytest -m json
```
Note that the script reads a config file **executables.ini** supposed contain the path executables (see above).
This file content is of the form : 
```ini
[executables]
antares-solver = /path/to/antares-solver
antares-problem-generator = /path/to/antares-problem-generator
```
