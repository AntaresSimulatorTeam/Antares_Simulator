import pytest

from check_on_results.check_general import check_list
from check_on_results.unfeasible_problem import unfeasible_problem, weeks_in_year
from check_on_results.sim_return_code import simulation_success

from fixtures import *

ALL_STUDIES_PATH = Path('../resources/Antares_Simulator_Tests_NR/unfeasible-problem').resolve()

# --------------------------------------------------------------
# Test behavior when an infeasible problem occurs
# --------------------------------------------------------------

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_01"], indirect=True)
def test_unfeasible_problem_01__error_verbose(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-verbose",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_01"], indirect=True)
def test_unfeasible_problem_01__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_01"], indirect=True)
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

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_01"], indirect=True)
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

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_02"], indirect=True)
def test_unfeasible_problem_02__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=6, weeks=[22,23,29]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_02"], indirect=True)
def test_unfeasible_problem_02__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=6, weeks=[22]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_03"], indirect=True)
def test_unfeasible_problem_03__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=6, weeks=[42,43,49]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=8, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
def test_unfeasible_problem_04(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_04"
    years_warning = {6: [22, 23, 29]}
    years_error = {6: [22]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_05(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_05"
    #Not all years with errors are tested, there is more than 200 years with errors
    years_warning = {10: [19], 11: [19], 16: [19],29: [19],  41: [42, 43, 49],43: [42, 43, 49],46: [42, 43, 49],142: [42, 43, 49]}
    years_error = {10: [19]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_06(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_06"
    years_warning = {3: [19], 6: [42, 43, 49], 7: [19], 8: [19]}
    years_error = {3: [19]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_07(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_07"
    years_warning = {1: [1], 3: [52], 5: [1], 7: [52], 9: [52]}
    years_error = {1: [1]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_08(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_08"
    years_warning = {3: [51],7: [51],9: [51]}
    years_error = {3: [51]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_09(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_09"
    years_warning = {1: [1], 3: [52], 5: [1], 7: [52], 9: [52]}
    years_error = {1: [1]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

@pytest.mark.unfeasible
def test_unfeasible_problem_10(solver_path):
    study_path = ALL_STUDIES_PATH / "specific-tests" / "unfeasible-problem" / "unfeasible_problem_10"
    years_warning = {3: [51],7: [51], 9: [51]}
    years_error = {3: [51]}
    check_all_unfeasible_options(solver_path, study_path, years_error, years_warning)

