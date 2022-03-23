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
def test_unfeasible_problem_01__error_verbose(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-verbose",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_01"], indirect=True)
def test_unfeasible_problem_01__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_01"], indirect=True)
def test_unfeasible_problem_01__warning_verbose(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=6, weeks=[42, 43, 49]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=8, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-verbose",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_01"], indirect=True)
def test_unfeasible_problem_01__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=6, weeks=[42, 43, 49]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=8, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

