import pytest
from pathlib import Path
import sys

from antares_test_utils import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()


# @pytest.fixture(autouse=True)
@pytest.fixture
def setup(study_path, solver_path, use_ortools, ortools_solver):
    # Actions done before the current test
    checkFactory = check_factory(study_path)
    studyExecutionFactory = study_execution_factory(study_path, solver_path, use_ortools, ortools_solver)
    checks = check_list()

    # Running the current test here
    yield checkFactory, checks

    # Teardown : actions done after the current test
    # ... Run the study
    study_exe = studyExecutionFactory.get(checks)
    study_exe.run()

    # ... Make all checks of the current test
    checks.run()

    print('End of test')



## TESTS ##
# @pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"], indirect=True)
def test_1(setup):
    checkFactory, checks = setup
    checks.add(check = checkFactory.get('output_compare'), system = 'win32')
    checks.add(check = checkFactory.get('integrity_compare'))
    checks.add(check = checkFactory.get('reservoir_levels'))


# @pytest.mark.short
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "short-tests" / "002 Thermal fleet - Base"], indirect=True)
def test_2(setup):
    checkFactory, checks = setup
    checks.add(check = checkFactory.get('integrity_compare'), system = 'win32')
    checks.add(check = checkFactory.get('unfeasible_problem'))
