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
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_03"], indirect=True)
def test_unfeasible_problem_03__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_04"], indirect=True)
def test_unfeasible_problem_04__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=6, weeks=[22,23,29]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_04"], indirect=True)
def test_unfeasible_problem_04__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=6, weeks=[22]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_05"], indirect=True)
def test_unfeasible_problem_05__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=10, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=11, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=16, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=29, weeks=[19]))
    warnings_on_weeks.append(weeks_in_year(year=41, weeks=[42,43,49]))
    warnings_on_weeks.append(weeks_in_year(year=43, weeks=[42,43,49]))
    warnings_on_weeks.append(weeks_in_year(year=46, weeks=[42,43,49]))
    warnings_on_weeks.append(weeks_in_year(year=142, weeks=[42,43,49]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_05"], indirect=True)
def test_unfeasible_problem_05__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=10, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_06"], indirect=True)
def test_unfeasible_problem_06__warning_dry(study_path, check_runner):
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
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_06"], indirect=True)
def test_unfeasible_problem_06__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[19]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_07"], indirect=True)
def test_unfeasible_problem_07__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=1, weeks=[1]))
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[52]))
    warnings_on_weeks.append(weeks_in_year(year=5, weeks=[1]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[52]))
    warnings_on_weeks.append(weeks_in_year(year=9, weeks=[52]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_07"], indirect=True)
def test_unfeasible_problem_07__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=1, weeks=[1]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_08"], indirect=True)
def test_unfeasible_problem_08__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[51]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[51]))
    warnings_on_weeks.append(weeks_in_year(year=9, weeks=[51]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_08"], indirect=True)
def test_unfeasible_problem_08__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[51]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_09"], indirect=True)
def test_unfeasible_problem_09__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=1, weeks=[1]))
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[52]))
    warnings_on_weeks.append(weeks_in_year(year=5, weeks=[1]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[52]))
    warnings_on_weeks.append(weeks_in_year(year=9, weeks=[52]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_09"], indirect=True)
def test_unfeasible_problem_09__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=1, weeks=[1]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)


@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_10"], indirect=True)
def test_unfeasible_problem_10__warning_dry(study_path, check_runner):
    warnings_on_weeks = []
    warnings_on_weeks.append(weeks_in_year(year=3, weeks=[51]))
    warnings_on_weeks.append(weeks_in_year(year=7, weeks=[51]))
    warnings_on_weeks.append(weeks_in_year(year=9, weeks=[51]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="warning-dry",
                                          checks_on_weeks=warnings_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)

@pytest.mark.unfeasible
@pytest.mark.parametrize('study_path', [ALL_STUDIES_PATH / "unfeasible_problem_10"], indirect=True)
def test_unfeasible_problem_10__error_dry(study_path, check_runner):
    errors_on_weeks = []
    errors_on_weeks.append(weeks_in_year(year=3, weeks=[51]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, new_behavior="error-dry",
                                          checks_on_weeks=errors_on_weeks,
                                          simulation=check_runner.get_simulation()))
    check_runner.run(checks)
