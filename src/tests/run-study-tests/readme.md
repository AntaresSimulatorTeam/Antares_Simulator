# How does the automatic testing script system work ?
## Introduction
Here is an automatic testing python script system.

This program performs the following : 
1. Searches for all studies in a given directory
2. From each study, retrieves the data to make further checks on that study (see items below)
3. Runs a simlulation on each study
4. Given the results of the simulation on a study, makes checks on them (comparison with reference resuts, check existence or content of output files,...)

Note that each study found is supposed to contain pieces of information about the kind of checks performed by scripts after the simulation on the study is completed.
So, each study is supposed to contain a file **check-config.json** for that purpose. This file is build manually for each study.

## Entry points to the automatic testing script system

For now, there are 2 entry points to run (end-to-end) tests automatically.
- test_from_json.py
- test_unfeasible_problem.py

The first one runs tests as we explained above, the second one is a bit different : it does not search for studies, but is given an explicit path to a study.
And the suplied study does not contain any check data : check are explicitly defined and supplied in the tests.

<span style="color:blue">TO DO : maybe we should think of integrating these tests in the regular testing system ?</span>

## File test_from_json.py
Let's look at the **test_from_json.py** script.
```python
1 @pytest.mark.json
2 @pytest.mark.parametrize('study_path, test_check_data', json_collector.pairs(), ids=json_collector.testIds())
3 def test(study_path, test_check_data, check_runner):
4    checks = create_checks(study_path, test_check_data, simulation=check_runner.get_simulation())
5    check_runner.run(checks)

```
**Line 1** : Following tests are marked as a collection, belonging to the same category

## Tests parametrization
**Line 2** : 

## Fixtures


# How to add a check on a test ?
