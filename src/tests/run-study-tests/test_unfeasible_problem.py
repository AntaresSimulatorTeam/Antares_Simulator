import pytest

from check_on_results.check_general import check_list
from check_on_results.unfeasible_problem import unfeasible_problem, weeks_in_year
from check_on_results.sim_return_code import simulation_success

from fixtures import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests').resolve()

# --------------------------------------------------------------
# Example of a test with output AND check integrity comparisons
# --------------------------------------------------------------

@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_01"], indirect=True)
def test_1(study_path, check_runner):

    weeks_list_error = []
    weeks_list_error.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-verbose", checks_on_weeks=weeks_list_error))
    checks.add(check = simulation_success(study_path, simulation=check_runner.get_simulation(), success_expected=False))
    check_runner.run(checks)

    weeks_list_warning = []
    weeks_list_warning.append(weeks_in_year(year=3, weeks=[19]))
    weeks_list_warning.append(weeks_in_year(year=6, weeks=[42, 43, 49]))
    weeks_list_warning.append(weeks_in_year(year=7, weeks=[19]))
    weeks_list_warning.append(weeks_in_year(year=8, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-verbose", checks_on_weeks=weeks_list_warning))
    checks.add(check = simulation_success(study_path, simulation=check_runner.get_simulation(), success_expected=True))
    check_runner.run(checks)

