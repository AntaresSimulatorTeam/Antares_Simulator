import pytest

from actions_on_study.study_run import *
from actions_on_study.results_remover import *
from check_on_results.check_general import check_list

class check_handler:
    def __init__(self, simulation, results_remover):
        self.simulation = simulation
        self.results_remover = results_remover
        self.checks = check_list()

    def get_simulation(self):
        return self.simulation

    def run(self, checks):
        self.simulation.run()

        # Store check list for teardown
        self.checks = checks
        checks.run()

    def teardown(self):
        study_modifiers = self.checks.study_modifiers()
        for modifier in study_modifiers:
            modifier.rewind()

        self.results_remover.run()

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


@pytest.fixture(autouse=True)
def check_runner(simulation, resutsRemover):
    # Actions done before the current test
    my_check_handler = check_handler(simulation, resutsRemover)

    # Running the current test here
    yield my_check_handler

    # Teardown : actions done after the current test
    my_check_handler.teardown()
