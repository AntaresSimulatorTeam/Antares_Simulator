import pytest
from pathlib import Path
import sys

from checks_on_results import *
from study_run import *
from results_remover import *
from print_results_handler import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

@pytest.fixture()
def study_path(request):
    return request.param

@pytest.fixture
def resutsRemover(study_path):
    return resuts_remover(study_path)

@pytest.fixture
def simulation(study_path, solver_path, use_ortools, ortools_solver):
    return study_run(study_path, solver_path, use_ortools, ortools_solver)

@pytest.fixture
def checks():
    return check_list()

@pytest.fixture
def printResults(study_path):
    return print_results_handler(study_path)

@pytest.fixture(autouse=True)
def setup(simulation, printResults, checks, resutsRemover):
    # Actions done before the current test
    # ==> nothing to run here

    # Running the current test here
    yield

    print() # To next line after current test status (PASSED or ERROR) is printed

    # Teardown : actions done after the current test
    printResults.enable_if_needed(checks)
    simulation.run()
    printResults.back_to_previous_state()

    # ... Make all checks of the current test
    checks.run()

    # ... Remove results on disk
    resutsRemover.run()

    print('End of test')



## TESTS ##
@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"], indirect=True)
def test_1(checks, study_path):
    print("test begins")
    checks.add(check = output_compare(study_path), system = 'win32')
    checks.add(check = integrity_compare(study_path))
    checks.add(check = reservoir_levels(study_path))




@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"], indirect=True)
def test_2(checks, study_path):
    checks.add(check = integrity_compare(study_path), system = 'win32')
    checks.add(check = unfeasible_problem(study_path))
    # raise AssertionError("Comparison failed")
