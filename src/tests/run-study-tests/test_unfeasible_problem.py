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
    # 3: [19], 6: [42, 43, 49], 7: [19], 8: [19]
    weeks_list = []
    weeks_list.append(weeks_in_year(year=3, weeks=[19]))
    weeks_list.append(weeks_in_year(year=6, weeks=[42, 43, 49]))

    checks = check_list()
    checks.add(check = unfeasible_problem(study_path, behavior="error-verbose", checks_on_weeks=weeks_list))
    checks.add(check = simulation_success(check_runner.get_simulation(), success_expected=False))
    check_runner.run(checks)