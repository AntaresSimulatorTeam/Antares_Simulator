import pytest
from pathlib import Path
import sys

from antares_test_utils import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

@pytest.fixture()
def study_path(request):
    return request.param

@pytest.fixture
def checkFactory(study_path):
    return check_factory(study_path)

@pytest.fixture
def studyExecutionFactory(study_path, solver_path, use_ortools, ortools_solver):
    return study_execution_factory(study_path, solver_path, use_ortools, ortools_solver)

@pytest.fixture
def checks():
    return check_list()

@pytest.fixture(autouse=True)
def setup(studyExecutionFactory, checks):
    # Actions done before the current test
    # ==> nothing to run here

    # Running the current test here
    yield

    print() # To next line after current test status (PASSED or ERROR) is printed

    # Teardown : actions done after the current test
    # ... Run the study
    study_exe = studyExecutionFactory.get(checks)
    study_exe.run()

    # ... Make all checks of the current test
    checks.run()

    print('End of test')



## TESTS ##
@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"], indirect=True)
def test_1(checkFactory, checks):
    checks.add(check = checkFactory.get('output_compare'), system = 'win32')
    checks.add(check = checkFactory.get('integrity_compare'))
    checks.add(check = checkFactory.get('reservoir_levels'))


@pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"], indirect=True)
def test_2(checkFactory, checks):
    checks.add(check = checkFactory.get('integrity_compare'), system = 'win32')
    checks.add(check = checkFactory.get('unfeasible_problem'))
