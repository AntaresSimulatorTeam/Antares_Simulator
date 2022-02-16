import pytest
from pathlib import Path
import sys

from antares_test_utils import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()


# @pytest.fixture(autouse=True)
@pytest.fixture
def setup(solver_path, use_ortools, ortools_solver):
    # Run before any test
    checkFactory = check_factory()
    studyExecutionFactory = study_execution_factory(solver_path, use_ortools, ortools_solver)
    checks = check_list()

    # Running any test here
    yield checkFactory, studyExecutionFactory, checks

    checkFactory.print_study_path()
    print('solver : %s' % solver_path)

    # Run after any test
    print('End of test')

## TESTS ##
# @pytest.mark.short
def test_1(setup):
    checkFactory, studyExecutionFactory, checks = setup
    study_path = ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"
    checkFactory.set_study_path(study_path)
    studyExecutionFactory.set_study_path(study_path)

    checks.add(check = checkFactory.get('output_compare'), system = 'win32')
    checks.add(check = checkFactory.get('integrity_compare'))
    checks.add(check = checkFactory.get('reservoir_levels'))


    # exe = get_execution_type(checks)
    # checks.run()


# @pytest.mark.short
def test_2(setup):
    checkFactory, studyExecutionFactory, checks = setup
    study_path = ALL_STUDIES_PATH / "short-tests" / "001 One node - passive"
    checkFactory.set_study_path(study_path)
    studyExecutionFactory.set_study_path(study_path)

    checks.add(check = checkFactory.get('output_compare'), system = 'win32')
    checks.add(check = checkFactory.get('unfeasible_problem'))
