import pytest

from actions_on_study.study_run import *
from actions_on_study.results_remover import *
from actions_on_study.print_results_handler import *

class check_handler:
    def __init__(self, simulation, results_remover):
        self.simulation = simulation
        self.results_remover = results_remover
        self.checks = None

    def get_simulation(self):
        return self.simulation

    def run(self, checks):
        self.simulation.run()

        # Store check list for teardown
        self.checks = checks
        checks.run()

        # In theory, removing results should occur in the teardown.
        # But making it happen here allows to call several simulation runs
        # in a single test and clear results after each run.
        self.results_remover.run()

    def teardown(self):
        study_modifiers = self.checks.study_modifiers()
        for modifier in study_modifiers:
            modifier.rewind()

# ================
# Fixtures
# ================
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
def printResults(study_path):
    return print_results_handler(study_path)


@pytest.fixture(autouse=True)
def check_runner(simulation, printResults, resutsRemover):
    # Actions done before the current test
    print("\nTest begins")
    my_check_handler = check_handler(simulation, resutsRemover)

    # Running the current test here
    yield my_check_handler

    # Teardown : actions done after the current test
    my_check_handler.teardown()
    print('\nEnd of test')