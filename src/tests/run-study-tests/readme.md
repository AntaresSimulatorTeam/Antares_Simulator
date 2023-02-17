# How does the automatic testing script system work ?
## Introduction
Here is an automatic testing python script system.

This program performs the following : 
1. Searches for all studies in a given directory
2. From each study, retrieves the specifications to make checks on the simulation results of that study (see items below)
3. Runs a simlulation on each study
4. Given the results of the simulation on a study, makes the checks retrieved at step 2 on these results (for instance : make sure current results and reference resuts are identical, check for existence or content of output files,...)

Note that each study found is supposed to contain the definition of checks performed by scripts after the simulation on the study is completed.
So, each study is supposed to contain a file **check-config.json** for that purpose. This file is build manually for each study.

## Entry points to the automatic testing script system

For now, there are 2 entry points that can be used to run tests automatically.
- test_from_json.py
- test_unfeasible_problem.py

The first one runs tests as we explained above, the second one is a bit different : it does not search for studies, but is given an explicit path to a study.
And this study does not contain any check definition under the form of a json file : checks are explicitly defined and supplied in the tests.

We won't comment the second script, as understanding the first one should ease understanding the second.

<span style="color:blue">TO DO : maybe we should think of integrating tests on unfeasible problems in the regular testing system ?</span>

## File test_from_json.py
Here we examine the second part of script **test_from_json.py**. Note that The first part is dedicated to finding studies and storing the definition of the checks to be made on results after a simulation is run on studies (recall the **check-config.json** file). 
Let's look at the **test_from_json.py** (second part).
```python
1 @pytest.mark.json
2 @pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
3 def test(study_path, test_check_data, check_runner):
4    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
5    check_runner.run(checks)
```
The content of this script is quite short, despite all the tests it has to perform. This is partially due to the power of **pytest** and its features : **fixtures** and especially **parametrization**, which is responsible for running a test many times, each time with a different set of arguments.

In the following, we comment the content of this script. Lines of this scripts are numbered so that they can be refered to whenever we need.

**Line 1** : Following tests are marked as a collection, that is belonging to the same category

## Fixtures
**pytest** comes with the notion of **fixture**. Fixtures allow executing a piece of code just before a test runs.
To take bebefit of a fixture, a test needs to be given this fixture as argument. 
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

Let's recall **line 2** :
```python
2 @pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
```
Here is the place where we allow calling the body of our test multiple times.
By "multiple time" we mean that the test will be run with different arguments each time.

In our example, fixture **study_path**, that is waiting for an argument, will be passed to the test, supplied with a different argument each time.

Same thing for the **test_check_data** value : it is not a feature (more a simple variable), but it will be passed to the test with a different value each time the test runs.

How do we do that ?

The first argument of the **parametrize** decorator (<span style="color:red">'study_path, test_check_data'</span>) represents the test's arguments to be changing each time the test runs.

The second argument is a list (**json_collector.pairs()**). Each element in the list is a pair (tuples with 2 elements) : 
- the first value of the pair is a path to a particular study
- the second value contains the data about checks that will be preformed on that study's results (after a simulation has run on that study).

So, a test is run for each element of this list. It receives the first value of the pair as first argument, and the second value of the pair as second argument.
This means that, for each test, fixture **study_path** receives as argument a path to a study, and that the variable **test_check_data** is supplied with an object containing all necessary data to performed a check.

Note : be aware that 2 pairs (study path, checks to do) can have the same study path : several checks can be made on the same study results.

<span style="color:blue">TO CHECK IN CODE  : for a given study, can we have many checks requiring as much as simulation runs ? My guess is yes, but to be checked</span>


## The test's body
With the previous explanations in mind, we're can describe what's happening when tests are run.

So the test is run multiple times due to **parametrization**. In fact, the test body is executed as many times as there are elements is the list **json_collector.pairs()**, that is as many studies spotted by the script **test_from_json.py** (see first part of the script, not commented in this doc).
### <ins>Run fixtures</ins>
Here we talk about **line 3** of script **test_from_json.py**.
```python
def test(study_path, test_check_data, check_runner):
    ...
```
For a given run of the test's body, that is for each study previously found in a directory, some fixtures are first run.
1. **study_path** : as already said above, this fixture is supplied with the path of the current study as only argument. Note that this fixture just returns this path, so this path is available for any use in the current test.
2. **test_check_data** : is not a fixture, more a variable containing a definition of the check to be made on the current study's results after a simulation is run on the study.
3. **check_runner** : this fixture does not seem to be the result of a parametrization, unlike the 2 first ones. So it seems it is called with the same argument for each study. It is not exactly the case because it calls other fixtures, that eventually call the **study_path** fixture (which is parametrized as we saw). 

Let's look at its content :
```python
@pytest.fixture(autouse=True)
def check_runner(simulation, resultsRemover):
    # Actions done before the current test
    my_check_handler = check_handler(simulation, resultsRemover)

    # Running the current test here
    yield my_check_handler

    # Teardown : actions done after the current test
    my_check_handler.teardown()
```
As we can see, this fixture is a **setup/teardown** fixture, and it calls 2 other fixtures (**simulation** and **resultsRemover**) before running.
So when **check_runner** runs : 
- A simulation is prepared (but not run yet)
- A remove of resuls associated to this simulation is prepared (but not run yet)
- A **check handler** object is returned to the current test, which will be able to use it under the form of its **check_runner** argument. We'll see that this **check handler** is responsible for running the simulation and for cleaning the simulation results after a test is completed.

Note that the last line here will be executed when the current test ends.

### <ins>Checks creation factory</ins>
This concerns **line 4** of script **test_from_json.py**.
```python
    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
```
Here we create a list of checks to perform on the results of a simulation on the current study is created.

This list is build from the description of the checks contained in **test_check_data**.

For that, **create_checks** is called, with natural information for a check : the path to the study used to fetch things to check, and te kind of thing to check (in **test_check_data**). The left argument of **create_checks** is the simulation previoulsy prepared by fixture **check_runner**. This simulation is needed for special kinds of check : the ones that need the return code of Antares Simulator just after it has run.

Note that all checks in that list is an instance of a class necessarily derived from the more general class **check_interface**. 
This parent class forced every child to have a method **run()** and a method **name()**, so that, when this list is traversed for any reason, a call to the **run()** or **name()** method does not fail.

Note also that, at this stage, the simulation has not been run yet, but will be run at the next line of **test_from_json.py**.

### <ins>Running the checks</ins>
This is about **line 5** of script **test_from_json.py**.
```python
    check_runner.run(checks)
```
This is the place where the script runs:
1. the simulation on the current study (this simulation was prepared when running fixture **check_runner** at test execution start)
2. the check(s) to be made on the results of this simulation (also prepared when fixture **check_runner** runs)

Indeed the **run()** method of the **check_runner** fixture contains these 2 instructions.

### <ins>End of tests</ins>
As we already saw, the **check_runner** fixture is a **setup/teardown** fixture. This means that when each test associated to a study ends, the **teardown** part of this fixture is run.

Looking at the content of **check_handler.teardown()** method, we see what it does :
- It puts back the study's state where it first was. Indeed, some checks modify the studies for their own purpose. The simulation is run with these new study parameters and checks are performed taking into account these changes. If several checks are made on the same study's results, and if every check requires a new simulation run, a tests can fail because a previous one made a change on the study that the current check does not expect. To avoid that, these changes are removed.
- It removes the simulation results. Again, if several checks are made on a study, that may imply as much associated simulation runs, and in these conditions, retrieving some results' data in a study that has several simulation results directories can lead to an unexpected failure in tests.

# How to add a check on a test ?
## What to do ?
1. Create a new python file in the **src/tests/run-study-tests/check_on_results**
2. In this file, add the definition of a new class associated to the new check on a simulation run. This class must be derived from the parent class **check_interface**. This parent class forces you to define a **run()** and a **name()** method to your check class.
3. You'll probably want to add your new check to one or more studies. In this case, you need to add your check to the associated **check-config.json**. This implies :
   - Defining a **json** representation of your check and how to insert it in a **check-config.json**
   - Adding these definitions in a **json schema**. Note that every found **check-config.json** is verified against the **json schema**
The schema can be found at : **src/tests/run-study-tests/parse_studies/json_schema.json**
4. Add the new check in the **check-config.json** files you need to.

## What NOT to do ?
- When defining the class for your check, do not supply the constructor with treatments that require a simulation run : in the testing script system, the check instances are created before the simulation is run.
- In the **json schema**, defining your check's grammar in the **"items" > "properties" > "checks" > "properties"** part of is not enough : you must as well declare it in the list of possible checks in the **"items" > "properties" > "checks"**

# Running the script

```bash
> cd src/tests/run-study-tests
> python -m pytest -m mps --solver-path=/path/to/the/Antares/solver/antares-x.y-solver.exe
```

# TO DO
Doc : Clarify if a check in the **check-config.json** can be composed of several sub-checks, and how it works in this case.
Code : 
